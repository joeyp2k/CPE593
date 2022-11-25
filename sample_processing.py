from nltk import tokenize
from nltk import ngrams

sample_file = open("sample.txt", "r")
words_file = open("words.txt", "w")
grams_file = open("grams.txt", "w")

for line in sample_file:
    line = tokenize.word_tokenize(line)
    line = [t for t in line if t.isalpha()]

    for word in line:
        words_file.write(word + "\n")
    
    grams = ngrams(line,2)
    grams = list(grams)

    for i in range(len(grams) - 2):
        gram_line = " ".join(grams[i])
        associated_word = grams[i+2][0]
        grams_file.write(gram_line + "." + associated_word + "\n")

    grams = ngrams(line,3)
    grams = list(grams)

    for i in range(len(grams) - 3):
        gram_line = " ".join(grams[i])
        associated_word = grams[i+3][0]
        grams_file.write(gram_line + "." + associated_word + "\n")

words_file.close()
grams_file.close()