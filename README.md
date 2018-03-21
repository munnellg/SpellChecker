# Spell Checker

An incredibly simple, stupid spell checker written over the course of an hour or two.

Marijn Schraagen's PhD thesis "Aspects of Record Linkage" presents a method of reducing the search space when comparing records in a database using bit vectors. The method translates very easily to a spell checker which is what is presented here.

Bit vectors encode strings as sequences of flags based on a vocabulary. For example, given the alphabet "abcd", the string "cab" would be encoded as { 1, 1, 1, 0 } i.e. it contains an a, b, and a c but not a d. "Dab" would be encoded as { 1, 1, 0, 1 }.

Bit vectors can be used to construct a tree which may be used to find terms that contain specific characters. Hence given a spelling error such as "the" misspelled as "teh", the tree may be searched for correct words that contain the letters t, e, and h. Search is followed by the application of a string similarity function that selects the most probable correction from a pool of candidates.

The search process can also allow for errors where characters are omitted or added to words accidentally, e.g. "exit" being misspelled "ext". Enabling this tolerance is simply a matter of flipping bits in the bit vector up to a maximum error while the tree is being searched.

Given a pool of candidate corrections retrieved from the tree, this program uses Jaro-Winkler distance to select the most likely spelling correction.

# Making and Running

The application may be built on Linux simply by calling make from the root directory. The binary takes one or two arguments as input. The first is mandatory and is a list of words to be added to the dictionary. These are stored in a case sensitive manner. 

If a second argument is provided then the contents of that file are used to evaluate the performance of the spell checker. Otherwise the application launches in interactive mode and allows you to query for correct spellings of words.

The format for executing the program is:

`./check dictionary.txt [evaluation.txt]`

# Performance

The standard datasets for evaluating a spell checker seems to be those surrounding Peter Norvig's [How to Write a Spelling Corrector](https://norvig.com/spell-correct.html). These are curated on [Kaggle](https://www.kaggle.com/bittlingmayer/spelling), but are not included in this repository. You will need to obtain them yourself.

The dictionary for corrections was built from the Debian Linux `/usr/share/dict/american-english` file. You will need to provide your own dictionary file if you want to test this application.

The total runtime and accuracy for the application on each of the spell checker datasets is reported below. The run times are extremely approximate as they include the time taken to build the dictionary and the time to check if a query term is "known".

The evaluation was performed using a case insensitive dictionary.

### aspell
```
Processed 531 queries: 0.514124% correct (24 unknown)

real	0m2.028s
user	0m1.988s
sys	0m0.036s
```

### birkbeck
```
Processed 36133 queries: 0.337392% correct (328 unknown)

real	1m28.432s
user	1m28.392s
sys	0m0.032s
```

### spell-testset1
```
Processed 270 queries: 0.659259% correct (4 unknown)

real	0m1.351s
user	0m1.316s
sys	0m0.032s
```

### spell-testset2
```
Processed 400 queries: 0.682500% correct (13 unknown)

real	0m2.055s
user	0m2.000s
sys	0m0.052s
```

### wikipedia
```
Processed 2455 queries: 0.710794% correct (85 unknown)

real	0m8.891s
user	0m8.852s
sys	0m0.036s
```