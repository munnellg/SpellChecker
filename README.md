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

The total runtime and accuracy for the application on each of the spell checker datasets is reported below. Note that this includes the time taken to actually build the dictionary, so the number of queries-per-second is a little higher than it may seem from these figures.

The evaluation was performed using a case insensitive dictionary.

### aspell
```
Processed 531 queries: 0.986817% correct

real	0m1.469s
user	0m1.432s
sys	0m0.040s
```

### birkbeck
```
Processed 36133 queries: 0.998367% correct

real	1m8.328s
user	1m8.244s
sys	0m0.076s
```

### spell-testset1
```
Processed 270 queries: 0.988889% correct

real	0m1.104s
user	0m1.076s
sys	0m0.028s
```

### spell-testset2
```
Processed 400 queries: 0.985000% correct

real	0m1.531s
user	0m1.496s
sys	0m0.032s
```

### wikipedia
```
Processed 2455 queries: 0.993890% correct

real	0m6.650s
user	0m6.600s
sys	0m0.048s
```