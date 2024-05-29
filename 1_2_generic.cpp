
///////////
//////////////
///////////////// generic
//////////////
///////////

#define LENOF(comptime_arr) \
    (sizeof(comptime_arr) / sizeof(*comptime_arr))

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
void vec_remove_if_exist(vector<T>& vec, const T& element) {
    if(vec_contains(vec, element)){
        auto it = find(vec.begin(), vec.end(), element);
        vec.erase(it);
    }
}

template<typename T>
T vec_get_random_element(vector<T>& vec) {
    int length = vec.size();
    assert(length > 0);
    int index = random_int(0, length - 1);
    return vec[index];
}

void term(char *command) {
    int ret_code = system(command);
    if(ret_code != 0){
        cerr << "ERROR: command failed: " << command << '\n';
        exit(1);
    }
}
