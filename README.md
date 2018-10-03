# Spell Checker

An incredibly simple, stupid spell checker written over the course of an hour or two.

Martin Schraagen's PhD thesis "Aspects of Record Linkage" presents a method of reducing the search space when comparing records in a database using bit vectors. The method translates very easily to a spell checker which is what is presented here.

Bit vectors encode strings as sequences of flags based on a vocabulary. For example, given the alphabet "abcd", the string "cab" would be encoded as { 1, 1, 1, 0 } i.e. it contains an 'a', 'b', and a 'c' but not a 'd'. "Dab" would be encoded as { 1, 1, 0, 1 }.

Bit vectors can be used to construct a tree which may be used to find terms that contain specific characters. Hence given a spelling error such as "the" misspelled as "teh", the tree may be searched for correct words that contain the letters 't', 'e', and 'h'. Search is followed by the application of a string similarity function that selects the most probable correction from a pool of candidates.

The search process can also allow for errors where characters are omitted or added to words accidentally, e.g. "exit" being misspelled "ext". Enabling this tolerance is simply a matter of flipping bits in the bit vector up to a maximum error while the tree is being searched.

Given a pool of candidate corrections retrieved from the tree, this program uses Jaro-Winkler distance to select the most likely spelling correction.

# Making and Running

The application may be built on Linux simply by calling `make` from the root directory. The binary takes one or two arguments as input. The first is mandatory and is a list of words to be added to the dictionary. These are stored in a case sensitive manner. 

If a second argument is provided then the contents of that file are used to evaluate the performance of the spell checker. Otherwise the application launches in interactive mode and allows you to query for correct spellings of words.

The format for executing the program is:

`./check dictionary.txt [evaluation.txt]`

# Performance

The standard datasets for evaluating a spell checker seems to be those surrounding Peter Norvig's [How to Write a Spelling Corrector](https://norvig.com/spell-correct.html). These are curated on [Kaggle](https://www.kaggle.com/bittlingmayer/spelling), but are not included in this repository. You will need to obtain them yourself.

The dictionary for corrections was built from the Debian Linux `/usr/share/dict/american-english` file. You will need to provide your own dictionary file if you want to test this application.

The total runtime and accuracy for the application on each of the spell checker datasets is reported below. The run times are extremely approximate as they include the time taken to build the dictionary and the time to check if a query term is "known".

The evaluation was performed using a case insensitive dictionary. Execution time was computed using the Linux `time` utility.

The evaluation produces two sets of scores:
+ The first is an overall evaluation of the performance of the spell checker. This includes errors where the correct word is not part of the dictionary i.e. it is impossible for the spell checker to find the correct word
+ The second excludes mistakes that are the result of words missing from the dictionary i.e. the spell checker should have found the right answer but did not.

In some cases a "misspelled" word will not be corrected because it is included in the dictionary. For example, for contextual reasons the word "dongle" should be corrected to "dangle". However, because "dongle" is a valid word, this spell checker will not correct it. A count of such cases is reported by the program.

### aspell
```
Processed 450 terms (24 unknown) : 531 queries (24 unknown)
20 "misspellings" were found in the dictionary
All Queries   : 0.514124 correct
Known Queries : 0.538462 correct

real	0m2.553s
user	0m2.512s
sys	0m0.036s
```

### birkbeck
```
Processed 6136 terms (328 unknown) : 36133 queries (952 unknown)
3782 "misspellings" were found in the dictionary
All Queries   : 0.337392 correct
Known Queries : 0.346522 correct

real	1m26.304s
user	1m26.168s
sys	0m0.124s
```

### spell-testset1
```
Processed 141 terms (4 unknown) : 270 queries (4 unknown)
5 "misspellings" were found in the dictionary
All Queries   : 0.659259 correct
Known Queries : 0.669173 correct

real	0m1.257s
user	0m1.208s
sys	0m0.044s
```

### spell-testset2
```
Processed 363 terms (13 unknown) : 400 queries (15 unknown)
16 "misspellings" were found in the dictionary
All Queries   : 0.682500 correct
Known Queries : 0.709091 correct

real	0m2.044s
user	0m2.004s
sys	0m0.036s
```

### wikipedia
```
Processed 1922 terms (85 unknown) : 2455 queries (98 unknown)
50 "misspellings" were found in the dictionary
All Queries   : 0.710794 correct
Known Queries : 0.740348 correct

real	0m11.102s
user	0m11.020s
sys	0m0.076s
```
