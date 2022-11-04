#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

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
            for(int i = 0; i < num_keys; i++){
                addition = addition + current_search->map[current_search->keys[i]]->key;
                cout << i << " " << current_search->map[current_search->keys[i]]->key << " " << addition << endl;
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
        void insert(string word, double frequency){
            CharacterMap *ptr = map;
            for(int i = 0; i < word.size(); i++){
                if(ptr->get_character(word[i])){
                    ptr = ptr->get_character(word[i]);
                }else{
                    ptr = ptr->add_character(word[i]);
                }
            }

            ptr->frequency = frequency;
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

wstring line;
wstring word;
double frequency;
wchar_t split = ',';
int main(){
    InputPrediction predictor = InputPrediction();

    predictor.insert("ampersand", 1);
    predictor.insert("acclaimed", 2);
    predictor.insert("accelerate", 5);
    predictor.insert("accent", 6);
    predictor.insert("actor", 3);
    predictor.insert("burger", 7);
    predictor.stream_suggested_words("ac");
}