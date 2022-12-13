#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string.h> 
#include <algorithm>

using namespace std;

class SugResult{
    public:
        string word;
        double frequency;
    SugResult(string w, double f){
        frequency = f;
        word = w;
    }
    ~SugResult(){
        
    }
};

class GramNode{
            public:
                string gram;
                double frequency;
                GramNode(string g){
                    gram = g;
                    frequency = 0.0;
                }
    ~GramNode(){

    }
};

class GramsFrequencyMap{
    private:
        int hashCode(string gram){
            int hash_index = 0;
            for(int i = 0; i < gram.size(); i++){
                hash_index += gram[i];
            }
            hash_index = hash_index % capacity;
            return hash_index;
        }

        int capacity;
        int size;

    public:
        vector<GramNode*> gram_map;
        vector<int> keys;
        
        void add_gram(string gram){
            int hash_code;

            if(capacity == size){
                expand_map();
            }

            hash_code = hashCode(gram);
            if(hash_code >= 0){
                while(gram_map[hash_code] != NULL){
                    hash_code++;
                    hash_code %= capacity;
                    if(gram_map[hash_code] != NULL && !gram.empty()){
                        if(gram == gram_map[hash_code]->gram){
                            gram_map[hash_code]->frequency++;
                            return;
                        }
                    }
                }

                GramNode *node = new GramNode(gram);

                gram_map[hash_code] = node;
                size++;
                keys.push_back(hash_code);
            }else{
                cout << "INVALID MAP CODE" << endl;
            }
        }

        void expand_map(){
            // cout << "Greater size needed" << endl;
            int prev_capacity = capacity;
            capacity = capacity = capacity + 5;

            vector<GramNode*> new_map(capacity);
            for (int i = 0; i < prev_capacity; i++){
                new_map[i] = gram_map[i];
            }
            for (int i = prev_capacity; i < capacity; i++){
                new_map[i] = NULL;
            }
            gram_map = new_map;
        }

        GramsFrequencyMap(int cap){
            capacity = cap;
            size = 0;
            gram_map = vector<GramNode*>(capacity);
            for (int i = 0; i < capacity; i++){
                gram_map[i] = NULL;
            }
        }
        ~GramsFrequencyMap(){

        }
};

class CharacterMap{
    private:
        class CharacterNode {
            public:
                char key;
                CharacterMap *value;
            CharacterNode(char k, CharacterMap *v){
                this->key = k;
                this->value = v;
            }
        };
        int capacity;
        int size;
        vector<int> keys;

        CharacterNode** map;

        int hashCode(char character){
            int hash_index = 0;

            hash_index += character;
            hash_index = hash_index % capacity;
            return hash_index;
        }

    public:
        double frequency;
        GramsFrequencyMap *grams;

        CharacterMap *add_character(char character){
            int hash_code;

            hash_code = hashCode(character);
            if(hash_code >= 0){
                while(map[hash_code] != NULL){
                    hash_code++;
                    hash_code %= capacity;
                }
                CharacterMap *char_map = new CharacterMap(26);
                CharacterNode *node = new CharacterNode(character,char_map);
                keys.push_back(hash_code);
                map[hash_code] = node;
                size++;
                return char_map;
            }else{
                cout << "INVALID MAP CODE" << endl;
                return NULL;
            }
        }

        CharacterMap *get_character(char character){
            int hash_code;
            int counter = 0;

            hash_code = hashCode(character);

            while(map[hash_code] != NULL){
                if(counter++ > capacity){
                    return NULL;
                }
                if(character == map[hash_code]->key){
                    return map[hash_code]->value;
                }
                hash_code++;
                hash_code %= capacity;
            }
        }

        void get_next_word(string input, string sug_word, string sug_word_last, CharacterMap *current_search, vector<SugResult> &results){
            int num_keys = current_search->keys.size();
            //search utilized keys and find words that have associated grams matching input
            if(current_search->frequency > 0.0){
                if(current_search->grams != NULL){
                    int num_keys_grams = current_search->grams->keys.size();
                    for(int j = 0; j < num_keys_grams; j++){
                        if(current_search->grams->gram_map[current_search->grams->keys[j]]->gram == input){
                            if(sug_word != sug_word_last){
                                SugResult sug = SugResult(sug_word,current_search->frequency);
                                results.push_back(sug);
                                sug_word_last = sug_word;
                            }
                        }
                    }
                }
            }
            for(int i = 0; i < num_keys; i++){
                string next_sug_word = sug_word + current_search->map[current_search->keys[i]]->key;
                get_next_word(input, next_sug_word, sug_word_last, current_search->map[current_search->keys[i]]->value, results);
            }
        }

        void complete_word(string root, string addition, string sug_word_last, CharacterMap *current_search, vector<SugResult> &results){
            int num_keys = current_search->keys.size();
            //search utilized keys
            if(current_search->frequency > 0.0){
                root = root + addition;
                if(root != sug_word_last){
                    sug_word_last = root;
                    SugResult sug = SugResult(root,current_search->frequency);
                    results.push_back(sug);
                    addition = "";
                }
            }
            for(int i = 0; i < num_keys; i++){
                string new_addition = addition + current_search->map[current_search->keys[i]]->key;
                complete_word(root, new_addition, sug_word_last, current_search->map[current_search->keys[i]]->value, results);
            }
        }
    
    CharacterMap(int n){
        capacity = n;
        grams = NULL;
		map = new CharacterNode*[capacity];
        size = 0;
        frequency = 0.0;
		for (int i = 0; i < capacity; i++){
			map[i] = NULL;
		}
    }
    ~CharacterMap(){

    }
};

class InputPrediction{
    private:
        CharacterMap *map;
    
        void quickSort(vector<SugResult> &results, int l, int r){
            if(l >= r){
                return;
            }
            int pivot_index = (l + r) / 2;
            SugResult pivot = results[pivot_index];

            int i = l;
            int j = r;

            while (i < j) {
                while (results[i].frequency > pivot.frequency){
                    i++;
                }
                while (results[j].frequency < pivot.frequency) {
                    j--;
                }
                if (i <= j) {
                    SugResult temp = results[i];
                    results[i] = results[j];
                    results[j] = temp;
                    i++;
                    j--;
                }
            }

            quickSort(results, l, j);
            quickSort(results, i, r);
        }
        
    public:
        void insert(string word){
            CharacterMap *ptr = map;
            for(int i = 0; i < word.size(); i++){
                if(ptr->get_character(word[i])){
                    ptr = ptr->get_character(word[i]);
                }else{
                    ptr = ptr->add_character(word[i]);
                }
            }

            ptr->frequency = ptr->frequency + 1;
        }

        void insertGram(string word, string gram_string){
            CharacterMap *ptr = map;
            for(int i = 0; i < word.size(); i++){
                if(ptr->get_character(word[i])){
                    ptr = ptr->get_character(word[i]);
                }else{
                    ptr = ptr->add_character(word[i]);
                }
                if(ptr->grams == NULL){
                    ptr->grams = new GramsFrequencyMap(5);
                }
                GramsFrequencyMap *grams = ptr->grams;
                grams->add_gram(gram_string);
            }
        }

        void stream_suggested_words(string input){
            string sug_word = "";
            vector<SugResult> results;
            CharacterMap *ptr = map;
            CharacterMap *ptr_last;
            int length = input.size();

            for(int i = 0; i < length; i++){
                //traverse the trie based on the input
                ptr_last = ptr;
                ptr = ptr->get_character(input[i]);
                if(ptr == NULL){
                    break;
                }

                sug_word += input[i];
            }

            if(length == 0){
                cout << "Enter a two/three words or a prefix" << endl;
                return;
            }else{
                if(ptr_last->frequency > 0){
                    //find the next word ordered by preceeding ngrams of the highest frequency
                    sug_word = "";
                    ptr_last->get_next_word(input,sug_word,"",map,results);
                }else if(ptr != NULL){
                    ptr->complete_word(sug_word,"","",ptr,results);
                }
            }
            
            int size = results.size();
            int output_len = 5;
            if(size > 1){
                quickSort(results,0,size - 1);
            }

            if(size < 5){
                output_len = size;
            }
            if(size > 0){
                cout << "TOP RESULTS FROM " << size << " WORDS FOUND" << endl;
                for(int i = 0; i < output_len; i++){
                    cout << results[i].word << " | f: " << results[i].frequency << endl;
                }
            }else{
                cout << "NO RESULTS FOUND" << endl;
            }
        }

    InputPrediction(){
        map = new CharacterMap(26);
    }
    ~InputPrediction(){

    }
};

int main(){
    InputPrediction predictor = InputPrediction();
    string input_string;
    string gram_entry;
    string token;
    string gram;
    string word;
    char split = '.';

    int counter = 0;

    ifstream textfile;

    textfile.open("words.txt");
    if(textfile.is_open()){
        while(getline(textfile, word)){
            predictor.insert(word);
        }
    }
    textfile.close();

    textfile.open("grams.txt");
    if(textfile.is_open()){
        while(getline(textfile, gram_entry)){
            stringstream s(gram_entry);

            getline(s,token,'.');
            gram = token;
            getline(s,token,'.');
            word = token;
            predictor.insertGram(word, gram);
        }
    }
    textfile.close();

    cout << "==== Text Input Prediction ====" << endl;
    cout << "Input a word prefix or a series of 2-3 words." << endl;
    // predictor.stream_suggested_words("a");
    // predictor.stream_suggested_words("r");
    while(true){
        getline(cin, input_string);
        predictor.stream_suggested_words(input_string);
        cout << endl;
    }
}