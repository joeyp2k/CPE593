#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string.h> 

using namespace std;

class GramNode{
            public:
                string gram;
                double frequency;
                GramNode(string g){
                    gram = g;
                    frequency = 0.0;
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
        vector<GramNode*> map;
        vector<int> keys;
        
        void add_gram(string gram){
            int hash_code;

            if(capacity == size){
                expand_map();
            }

            hash_code = hashCode(gram);
            if(hash_code >= 0){
                while(map[hash_code] != NULL){
                    hash_code++;
                    hash_code %= capacity;
                    if(map[hash_code] != NULL && !gram.empty()){
                        if(gram == map[hash_code]->gram){
                            map[hash_code]->frequency++;
                            return;
                        }
                    }
                }

                GramNode *node = new GramNode(gram);

                map[hash_code] = node;
                size++;
                keys.push_back(hash_code);
            }else{
                cout << "INVALID MAP CODE" << endl;
            }
        }

        void expand_map(){
            cout << "Greater size needed" << endl;
            int prev_capacity = capacity;
            capacity = capacity = capacity + 5;

            vector<GramNode*> new_map(capacity);
            for (int i = 0; i < prev_capacity; i++){
                new_map[i] = map[i];
            }
            for (int i = prev_capacity; i < capacity; i++){
                new_map[i] = NULL;
            }
            map = new_map;
        }

        GramsFrequencyMap(int cap){
            capacity = cap;
            size = 0;
            map = vector<GramNode*>(capacity);
            for (int i = 0; i < capacity; i++){
                map[i] = NULL;
            }
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
        
        void get_next_word(string input, string sug_word, CharacterMap *current_search){
            int num_keys = current_search->keys.size();
            //search utilized keys and find words that have associated grams matching input
            for(int i = 0; i < num_keys; i++){
                get_next_word(input, sug_word, current_search->map[current_search->keys[i]]->value);
                if(current_search->grams != NULL){
                    int num_keys_grams = current_search->grams->keys.size();
                    sug_word = sug_word + current_search->map[current_search->keys[i]]->key;
                    for(int j = 0; j < num_keys_grams; j++){
                        if(current_search->grams->map[current_search->grams->keys[j]]->gram == input){
                            //order by frequency
                            cout << sug_word << endl;
                            sug_word = "";
                        }
                    }
                }
            }
        }

        void complete_word(string root, string addition, CharacterMap *current_search){
            int num_keys = current_search->keys.size();
            //search utilized keys
            if(current_search->frequency > 0.0){
                //order by frequency
                cout << root + addition << endl;
                root = root + addition;
                addition = "";
            }
            for(int i = 0; i < num_keys; i++){
                string new_addition = addition + current_search->map[current_search->keys[i]]->key;
                cout << i << " " << current_search->map[current_search->keys[i]]->key << " " << root << " " << addition << endl;
                complete_word(root, new_addition, current_search->map[current_search->keys[i]]->value);
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
            }
            
            //add gram and increment frequency
            if(ptr->grams == NULL){
                ptr->grams = new GramsFrequencyMap(5);
            }
            GramsFrequencyMap *grams = ptr->grams;
            grams->add_gram(gram_string);
        }

        void stream_suggested_words(string input){
            string sug_word = "";
            CharacterMap *ptr = map;
            CharacterMap *ptr_last;
            string sug_word_last = "";
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
            if(length > 1){
                 if(ptr_last->frequency > 0){
                    //find the next word ordered by preceeding ngrams of the highest frequency
                    sug_word = "";
                    ptr_last->get_next_word(input,sug_word,map);
                }else{
                    //find the words ordered by the highest frequency under this map
                    sug_word = sug_word.substr(0, sug_word.size() - 1);
                    ptr_last->complete_word(sug_word,"",ptr_last);
                }
            }else{
                 ptr->complete_word(sug_word,"",ptr);
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
    cout << "Input a partially completed word or a series of words." << endl;
    while(true){
        cin >> input_string;
        predictor.stream_suggested_words(input_string);
    }
}