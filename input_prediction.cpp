#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>

using namespace std;

class GramNode{
            public:
                string gram;
                int count;
                GramNode(string g){
                    gram = g;
                    count = 0;
                }
        };

class GramsFrequencyMap{
    private:
        int hashCode(string gram){
            int hash_index = 0;
            for(int i = 0; i < gram.size(); i++){
                hash_index += gram[i];
            }

            return hash_index % capacity;
        }

        int capacity;
        int size;

    public:
        GramNode **map;
        vector<int> keys;
        
        void add_gram(string gram){
            int hash_code;

            hash_code = hashCode(gram);
            if(hash_code >= 0){
                while(map[hash_code] != NULL){
                    hash_code++;
                    hash_code %= capacity;
                }

                GramNode *node = new GramNode(gram);

                map[hash_code] = node;
                size++;
            }
            cout << "INVALID MAP CODE" << endl;
        }

        GramNode *get_gram_from_code(int code){
            return map[code];
        }

        int get_gram(string gram){
            int hash_code;
            int counter = 0;

            hash_code = hashCode(gram);

            while(map[hash_code] != NULL){
                if(counter++ > capacity){
                    return -1;
                }
                if(gram == map[hash_code]->gram){
                    map[hash_code]->count++;
                    return hash_code;
                }
                hash_code++;
                hash_code %= capacity;
            }
        }

        void insert_gram(string gram){
            int code = get_gram(gram);
            if(code == -1){
                add_gram(gram);
            }
            if(capacity == size){
                expand_map();
            }
        }

        void expand_map(){

        }

        GramsFrequencyMap(int cap){
            capacity = cap;
            map = new GramNode*[capacity];
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

            return hash_index % capacity;
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
                CharacterMap *char_map = new CharacterMap(50);

                CharacterNode *node = new CharacterNode(character,char_map);

                keys.push_back(hash_code);
                map[hash_code] = node;
                size++;
                return char_map;
            }
            cout << "INVALID MAP CODE" << endl;
            return NULL;
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

        void get_suggested_words(string root, string addition, CharacterMap *current_search){
            int num_keys = current_search->keys.size();
            //search utilized keys
            for(int i = 0; i < num_keys; i++){
                addition = addition + current_search->map[current_search->keys[i]]->key;
                cout << i << " " << current_search->map[current_search->keys[i]]->key << " " << root << " " << addition << endl;
                get_suggested_words(root, addition, current_search->map[current_search->keys[i]]->value);
                addition = "";
            }
            if(current_search->frequency > 0.0){
                cout << root + addition << endl;
            }
        }
    
    CharacterMap(int n){
        capacity = n;
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
            
            //insert gram and increment count
            GramsFrequencyMap *grams = ptr->grams;
            grams->insert_gram(gram_string);
        }

        void stream_suggested_words(string input){
            string sug_word = "";
            CharacterMap *ptr = map;
            int length = input.size();
            for(int i = 0; i < length; i++){
                //traverse the trie based on the input
                ptr = ptr->get_character(input[i]);
                if(ptr == NULL){
                    break;
                }
                sug_word += input[i];
            }
            //find the words ordered by the highest frequency under this map
            ptr->get_suggested_words(sug_word,"",ptr);
        }

    InputPrediction(){
        map = new CharacterMap(50);
    }
    ~InputPrediction(){

    }
};

double frequency;
wchar_t split = ',';

int main(){
    InputPrediction predictor = InputPrediction();
    GramsFrequencyMap gram_freq_map = GramsFrequencyMap(10000);

    string gram2;
    string gram3;
    string last_word;
    string second_last_word;
    string word;

    int counter = 0;

    ifstream textfile;

    textfile.open("");

    //load grams frequency map and predictor
    if(textfile.is_open()){
        while(getline(textfile, word)){
            predictor.insert(word);
            if(counter > 2){
                predictor.insertGram(word,gram2);
            }
            if(counter > 3){
                predictor.insertGram(word,gram3);
            }else{
                counter++;
            }
            
            gram2 = word + " " + last_word;
            gram3 = word + " " + last_word + " " + second_last_word;
        }
    }
    textfile.close();

    predictor.insert("ampersand");
    predictor.insert("acclaimed");
    predictor.insert("accelerate");
    predictor.insert("accent");
    predictor.insert("actor");
    predictor.insert("burger");
    predictor.stream_suggested_words("ac");
}