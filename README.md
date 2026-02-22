# Pipeline architechture
1. Almost all compressors work on model + coding logic. The model for this compression algorithm assume that the input data it is trying to compress has local corelation which can be exploited by Burrows-Wheeler-transform(BWT).

2. After BWT it applies Move-To-Front transform which further increase local corelation.
3. By local corelation I mean the fact that, if we see a symbol say 'a' it is very like that the next symbol is say 'b'.

4. English and other languages in particular have this structure, for e.g. if we see "th" it is very likely that the nexy symbol is going to be 'e'.

5. After move to front the next stage is Run-length encoding. This algorithm uses a custom RLE scheme.

6.Finally, the algorithm applies entropy encoding, in this case - huffman encoding to the meta data blocks and data blocks of the RLE stage

# Results

All the sizes are in Bytes

#### Calgary corpus
| Filename | Original Size | Compressed Size | Improvement |
| :--- | :--- | :--- | :--- |
| bib | 111261 | 33671 | 69.73 |
| book1 | 768771 | 293884 | 61.77 |
| book2 | 610856 | 197488 | 67.67 |
| geo | 102400| 69445 | 32.18 |
| news | 377109 | 145201 | 61.50 |
| obj1 | 21504 | 12276 | 42.91 |
| obj2 | 246814 | 89905 | 63.57 |
| paper1 | 53161 | 20318 | 61.78 |
| paper2 | 82199 | 31017 | 62.27 |
| paper3 | 46526 | 19472 | 58.15 |
| paper4 | 13286 | 6292 | 52.64 |
| paper5 | 11954 | 5816 | 51.34 |
| paper6 | 38105 | 15076 | 60.43 |
| pic | 513216 | 61772 | 87.96 |
| progc | 39611 | 15264 | 61.46 |
| progl | 71646 | 19636 | 72.59 |
| progp | 49379 | 13397 | 72.87 |
| trans | 93695 | 22302| 76.20 |


#### Canterbury corpus
| Filename | Original Size | Compressed Size | Improvement |
| :--- | :--- | :--- | :--- |
| alice29.txt | 152089 | 53997 | 64.50 |
| asyoulik.txt | 125179 | 49409 | 60.52 |
| cp.html | 24603 | 9099 | 63.01 |
| fields.c | 11150 | 3742 | 66.44 |
| grammar.lsp | 3721 | 1567 | 57.88|
| kennedy.xls | 1029744 | 107362 | 89.57 |
| lcet10.txt | 426754 | 135768 | 68.18 |
| plrabn12.txt | 481861 | 183909 | 61.83 |
| ptt5 | 513216 | 61772 | 87.96 |
| sum | 38240| 15325 | 59.92 |
| xargs | 4227 | 2098 | 50.37 |


#### Miscellaneous
| Filename | Original Size | Compressed Size | Improvement |
| :--- | :--- | :--- | :--- |
| pi.txt | 1000000 | 530424 | 46. 96|
| random.txt | 100000 | 86930 | 13.07|
| enwik8 | 100000000 | 29508101 | 70.49|
| enwik9 | 1000000000 | 234968102 | 76.50 |
| aaa.txt | 100000| 65 | 99.93 |
| alphaber.txt | 100000| 162 | 99.83 |


The result of miscellaneous are as expected.
1. pi.txt contains the first million digits of pi. pi is only made of 10 symbols (10 digits) which can fit in 4 bit words each, therfore even a naive algorithm would give us 50 percent reduction in size, better than out algorithm. This is to be expected though, because there very little co-relation in pi, for our model of compression it just appears random.

2. random.txt is very poorly compressed, which is assuring since its supposed to be random and therefore, generaly not compressible

3. enwik8 and enwik9 are are the first 100 MB and 1GB of english wikipedia, These are surprisingly compressed better than other english texts in calgary and canterbury corpus.

4. aaa.txt and alphabet.txt are repeated string of 'a' and the english alphabet, which is the reason why there is so much redundancy in them and hence highly compressible.















