#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define BUF_MAX      64
#define VECTOR_SIZE  128
#define MATCH_THRESH 0.90

#define JW_COMMON_PREFIX  4
#define JW_SCALING_FACTOR 0.1

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))

struct vector_node {	
	struct vector_node *child[2], *next;
	int weight;
	char term[BUF_MAX];
};

double
jaro_winkler ( char *s1, char *s2 ) {	
	int i, j, lo, hi;	
	int slen1, slen2, window;	
	double match = 0, trans = 0, jaro;

	if ( !s1 || !s2 ) { return 0.0; }

	slen1 = strlen(s1);
	slen2 = strlen(s2);	
	if ( !slen1 || !slen2 ) { return 0.0; }

	int s1match[slen1], s2match[slen2];
	memset(s1match, 0, sizeof(int)*slen1);
	memset(s2match, 0, sizeof(int)*slen2);

	window = MAX(slen1, slen2)/2 - 1;
	for ( i = 0; i < slen1; i++ ) {
		lo = MAX( 0, i - window );
		hi = MIN( slen2, i + window + 1 );

		for ( j = lo; j < hi; j++ ) {
			if ( s1[i] == s2[j] && !s2match[j] ) {
				s1match[i] = 1;
				s2match[j] = 1;
				match++;				
				break;
			}
		}
	}

	if ( !match ) { return 0.0; }
	
	for ( i = 0, lo = 0; i < slen1; i++ ) {
		if ( s1match[i] ) {
			while (lo < slen2 && !s2match[lo]) { lo++; }
			trans += (s1[i] != s2[lo++]);
		}
	}

	trans *= 0.5;

	// Compute Jaro based on matches and transpositions
	jaro = ( match/slen1 + match/slen2 + (match-trans)/match ) / 3.0;

	// Compute length of common prefix upto a max length of 4
	match = 0;
	hi = MIN(MIN(slen1, slen2), JW_COMMON_PREFIX);
	for ( i = 0; i<hi; i++ ) {
		match += (s1[i] == s2[i]);
	}

	// compute Jaro-Winkler distance and return
	return jaro + match * JW_SCALING_FACTOR * ( 1 - jaro );
}

void
vectorize ( char *str, int *v, int vector_size ) {
	memset( v, 0, sizeof(int) * vector_size );
	for ( char *p = str; *p; p++ ) {
		int i = *p;
		if ( i >= 0 && i < vector_size ) {
			v[i] = 1;
		}
	}
}

void
vector_node_add ( struct vector_node **n, char *str, int *v, int vector_size,
		int depth ) {
	struct vector_node *p = *n;

	if ( !p ) {
		p = calloc(1, sizeof(struct vector_node));
		if ( !p ) { fprintf(stderr, "calloc failed\n"); return; }
		*n = p;
	}

	if ( depth < vector_size ) {	
		vector_node_add( &p->child[v[depth]], str, v, vector_size, depth + 1 );
	} else {
		struct vector_node *tmp = calloc(1, sizeof(struct vector_node));
		if ( !tmp ) { fprintf(stderr, "calloc failed\n"); return; }
		memcpy( tmp->term, str, sizeof(char)*strlen(str));
		tmp->next = p->next;
		p->next = tmp;
	}
}

void
vector_node_get ( struct vector_node **result, struct vector_node *dict,
		int *v, int vector_size, int depth, int err1, int err2, int errmax ) {
	struct vector_node *r = *result;
	
	if ( !dict ) { return; }

	if ( depth >= vector_size ) {
		struct vector_node *p = dict->next;		
		while (p) {
			struct vector_node *tmp = calloc(1, sizeof(struct vector_node));
			if ( !tmp ) { fprintf(stderr, "calloc failed\n"); return; }
			memcpy( tmp->term, p->term, sizeof(char)*strlen(p->term));
			tmp->next = r;
			r = tmp;
			p = p->next;
		}

		*result = r;
		return;
	}

	int bit = v[depth];

	if ( errmax >= err1 + bit && errmax >= err2 + (1-bit) ) {		
		vector_node_get( result, dict->child[1-bit], v, vector_size, depth + 1,
			err1 + bit, err2 + (1-bit), errmax );		
	}

	vector_node_get( result, dict->child[bit], v, vector_size, depth + 1,
		err1, err2, errmax);
}

void
vector_node_free ( struct vector_node *n ) {
	if (n) {
		vector_node_free(n->child[0]);
		vector_node_free(n->child[1]);
		vector_node_free(n->next);
		free(n);
	}	
}

struct vector_node*
load_dictionary ( char *fname ) {
	FILE *f;
	char buf[BUF_MAX];
	int v[VECTOR_SIZE];	
	struct vector_node *dict = NULL;

	f = fopen(fname, "r");
	if (!f) {
		fprintf(stderr, "Unable to open \"%s\"\n", fname);
		return NULL;
	}

	while( fscanf(f, "%s", buf) != EOF ) {		
		vectorize(buf, v, VECTOR_SIZE);
		vector_node_add( &dict, buf, v, VECTOR_SIZE, 0 );
	}

	fclose(f);

	return dict;
}

double
best_match ( struct vector_node *dict, char *str, char *match, int max_len ) {
	double jw, jw_max;
	int v[VECTOR_SIZE];
	struct vector_node *results, *p;
	
	results = NULL;
	vectorize(str, v, VECTOR_SIZE);
	vector_node_get( &results, dict, v, VECTOR_SIZE, 0, 0, 0, 2 );

	jw_max = 0;
	p = results;
	while (p) {
		jw = jaro_winkler(p->term, str);
		if ( jw > jw_max ) {
			jw_max = jw;
			strncpy( match, p->term, max_len );
		}
		p = p->next;
	}

	vector_node_free(results);

	return jw_max;
}

void
query ( struct vector_node *dict ) {	
	double jw;
	char buf[BUF_MAX], match[BUF_MAX];	
	

	for (;;) {

		memset(buf, 0, sizeof(char) * BUF_MAX);
		fprintf(stdout, ">>> ");
		fscanf(stdin, "%s", buf);

		if ( strcmp(buf, "\\q") == 0 ) { break;	}

		jw = best_match( dict, buf, match, BUF_MAX );

		if ( jw == 1 ) {
			fprintf( stdout, "Found \"%s\" in the dictionary\n", match );
		} else if ( jw > MATCH_THRESH ) {
			fprintf( stdout, "Best match \"%s\"\n", match );
		} else {
			fprintf( stdout, "No matches found\n" );
		}
	}
}

void
evaluate ( struct vector_node *dict, char *fname ) {
	char target[BUF_MAX], query[BUF_MAX], result[BUF_MAX];
	int total_terms = 0, known_terms = 0;	
	int total_query_count = 0, total_correct = 0;
	int known_query_count = 0, known_correct = 0;
	int is_correct = 0, is_unknown = 0;

	FILE *f;

	f = fopen(fname, "r");
	if (!f) {
		fprintf(stderr, "Unable to open \"%s\"\n", fname);
		return;
	}

	while ( fscanf(f, "%s", query) != EOF ) {		
		if ( strrchr(query, ':') ) {
			strncpy( target, query, strlen(query)-1 );
			target[strlen(query)-1] = 0;
			is_unknown = (best_match( dict, target, result, BUF_MAX ) != 1);
			total_terms++;
			if ( !is_unknown ) { known_terms++; }
		} else {
			best_match( dict, query, result, BUF_MAX );
			is_correct = (strcmp( result, target ) == 0 );

			total_query_count++;			
			total_correct += is_correct ;

			if ( !is_unknown ) {
				known_query_count++;
				known_correct += is_correct;
			}
		}
	}

	fclose(f);

	printf("Processed %d terms (%d unknown) : %d queries (%d unknown)\n",
		total_terms, total_terms - known_terms, 
		total_query_count, total_query_count - known_query_count
	);
	
	printf("All Queries   : %f correct\n",
		(float) total_correct / total_query_count
	);

	printf("Known Queries : %f correct\n",
		(float) known_correct / known_query_count
	);
}

int
main ( int argc, char *argv[] ) {	
	struct vector_node *dict = NULL;

	if ( argc < 2 ) {
		fprintf(stderr, "usage: %s DICT [EVALUATION]\n", argv[0]);
		return EXIT_FAILURE;
	}

	dict = load_dictionary(argv[1]);
	
	if ( argc > 2 ) {
		evaluate(dict, argv[2]);
	} else {
		query(dict);	
	}
	
	vector_node_free(dict);

	return EXIT_SUCCESS;
}
