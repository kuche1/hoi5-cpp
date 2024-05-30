
///////////
//////////////
///////////////// generic
//////////////
///////////

#define LENOF(comptime_arr) \
    (sizeof(comptime_arr) / sizeof(*comptime_arr))

///// random

float random_0_to_1() {
    // Seed the random number generator
    random_device rd;
    mt19937 gen(rd());
    
    // Define a uniform distribution over the range [0, 1]
    uniform_real_distribution<float> dist(0.0, 1.0);
    
    // Generate a random number
    float randomNum = dist(gen);

    return randomNum;
}

// range is inclusive
int random_int(int from, int to) {
    // Random number generation setup
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(from, to); // Define the range

    // Generate a random index
    int random_number = distr(gen);

    return random_number;
}

///// vector

template<typename T>
bool vec_contains(const vector<T>& vec, const T& element) {
    return find(vec.begin(), vec.end(), element) != vec.end();
}

template<typename T>
void vec_push_back_nodup(vector<T>& vec, const T& element) {
    if(!vec_contains(vec, element)){
        vec.push_back(element);
    }
}

template<typename T>
void vec_remove(vector<T>& vec, const T& element) {
    assert(vec_contains(vec, element));

    auto it = find(vec.begin(), vec.end(), element);
    vec.erase(it);
}

template<typename T>
void vec_remove_if_exist(vector<T>& vec, const T& element) {
    if(vec_contains(vec, element)){
        vec_remove(vec, element);
    }
}

template<typename T>
int vec_get_index(vector<T>& vec, const T& element) 
{ 
    auto it = find(vec.begin(), vec.end(), element); 
  
    // If element was found 
    if (it != vec.end()) {
        // calculating the index 
        int index = it - vec.begin(); 
        return index;
    } 
    else { 
        // If the element is not 
        // present in the vector 
        assert(false);
    }
}

template<typename T0, typename T1>
vector<tuple<T0, T1>> vec_zip(vector<T0>& vec0, vector<T1>& vec1) {

    assert(vec0.size() == vec1.size());
    int length = vec0.size();

    vector<tuple<T0, T1>> result = {};

    for(int i=0; i<length; ++i){
        T0 elem0 = vec0[i];
        T1 elem1 = vec1[i];
        tuple<T0, T1> combined = make_tuple(elem0, elem1);
        result.push_back(combined);
    }

    return result;
}

template<typename T>
T vec_get_random_element(vector<T>& vec) {
    int length = vec.size();
    assert(length > 0);
    int index = random_int(0, length - 1);
    return vec[index];
}

///// map

template<typename T_key, typename T_value>
bool map_contains(map<T_key, T_value> map, T_key key){
    if (map.find(key) != map.end()) {
        return true;
    }
    return false;
}

///// terminal

void term(char *command) {
    int ret_code = system(command);
    if(ret_code != 0){
        cerr << "ERROR: command failed: " << command << '\n';
        exit(1);
    }
}
