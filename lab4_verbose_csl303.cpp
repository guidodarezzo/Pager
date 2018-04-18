//
//  main.cpp
//  OSLab4
//
//  Created by Caroline Lee on 4/13/16.
//  Copyright © 2016 Caroline Lee. All rights reserved.
//
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <streambuf>
#include <vector>
#include <cmath>
#include <algorithm>
#include <deque>
#include <climits>

using namespace std;

std::vector<int> randomNumbers;
int randomIndex = -1;
void createRandomNumVector();
int get_next_word(int proc);
int randomOS(int max);
class Pager;

int M, P, S, J, N, num_proc, num_frames;
std::vector<float> A, B, C, residency_time;
std::vector<int> page_faults, last_word, num_replacements, next_words;
string R;

struct frame {
    bool present;
    int page;
    int process;
    int page_in;
};

std::vector<frame> frame_table;


void createRandomNumVector(){
    
    ifstream infile;
    infile.open("random-numbers.txt");
    
    int d;
    while (infile >> d){
        randomNumbers.push_back(d);
    }
    
    
} // end createRandomNumVector

int randomOS(int max){
    randomIndex++;
    cout << randomIndex << endl ;
    int randomNumber = ((randomNumbers[randomIndex])%max);
    return randomNumber;
} // end randomOS

class Pager {
public:
    virtual int find_replacement() {
        cout << "Running Nothing" << endl;
        return 0;
    }
    virtual void notify_referenced(int frame) {
        cout << "Running Nothing" << endl;

    }
    virtual void notify_paged_in(int frame){
        cout << "Running Nothing" << endl;
    }
};

class FIFOpager : public Pager {
public:
    std::deque<int> frame_queue;
    int find_replacement() {
        int to_replace = frame_queue.front();
        frame_queue.pop_front();
        return to_replace;
    }
    
    void notify_paged_in(int frame){
        frame_queue.push_back(frame);
    }
    
    void notify_referenced(int frame){

    }
    FIFOpager() {};
};



class LRUpager : public Pager {
public:
    std::deque<int> frame_prio;
    int find_replacement() {
        int to_replace = frame_prio.front();
        frame_prio.pop_front();
        return to_replace;
    }
    
    void notify_paged_in(int frame){
        frame_prio.push_back(frame);

    }
    
    void notify_referenced(int frame){
        for (int i = 0 ; i < frame_prio.size() ; i++) {
            if (frame == frame_prio[i]) {
                frame_prio.erase(frame_prio.begin() + i);
                frame_prio.push_back(frame);
                break;
            }
        }
    }
    LRUpager() {};
    
};

class RANDpager : public Pager {
public:
    int find_replacement() {
        return randomOS(num_frames);
    }
    
    void notify_paged_in(int frame){
        
    }
    
    void notify_referenced(int frame){
        
    }
    RANDpager() {};
    
};


int get_next_word(int proc) {
    int rand = randomOS(INT_MAX);
    int w = last_word[proc];
    double y = rand/(INT_MAX+1.0);
    if (y < A[proc])
        return (w + 1) %S;
    if (y < A[proc] +  B[proc])
        return (w-5+S) % S;
    if (y < A[proc] + B[proc] + C[proc])
        return (w+4)% S;
    return  (int)randomOS(S);
}



int main(int argc, const char * argv[]) {
    Pager* pager;
    M = atoi(argv[1]);
    P = atoi(argv[2]);
    S = atoi(argv[3]);
    J = atoi(argv[4]);
    N = atoi(argv[5]);
    R = argv[6];
    
    cout << "M: " << M << endl;
    cout << "P: " << P << endl;
    cout << "S: " << S << endl;
    cout << "J: " << J << endl;
    cout << "N: " << N << endl;
    cout << "R: " << R << endl;
    
    if (string("random").compare(R) == 0) {
        cout << "Running Random" << endl;
        //RANDpager randpager;
        //pager = randpager;

        pager = new RANDpager();
    }
    if (string("fifo").compare(R) == 0) {
        cout << "Running Fifo" << endl;
        // FIFOpager  fifopager;
        // fifopager->find_replacement();
        // pager = fifopager;
        // pager->find_replacement();

        pager = new FIFOpager();
    }
    if (string("lru").compare(R) == 0) {
        cout << "Running LRU" << endl;
        // LRUpager lrupager;
        // pager = lrupager;

        pager = new LRUpager();
    }
    num_frames = M/P;
    
    cout << "num_frames = " << num_frames << endl;

    
    for (int i = 0 ; i < num_frames ; i++) {
        frame new_frame;
        new_frame.present = false;
        frame_table.push_back(new_frame);
    }
    
    
    cout << "before random num call..." << endl;
    
    
    createRandomNumVector();
    cout << randomNumbers.size() << endl;
    
    for (int i = 0; i < 5; i++) {
        cout << "Random number " << i + 1 << ": " << randomNumbers[i] << endl;
    }
    
    cout << "after random num call..." << endl;
    int initial_words[4] = {111 % S, 222 % S, 333 % S, 444 % S};
    float f_zeros[4] = {0.0,0.0,0.0,0.0};
    int i_zeros[4] = {0,0,0,0};
    int l[4] = {1,2,3,4};
    if (J == 1) {
        num_proc = 1;
        last_word.push_back(1);
        residency_time.push_back(0.0);
        num_replacements.push_back(0);
        page_faults.push_back(0);
        next_words.push_back(111 % S);
        A.push_back(1.0);
        B.push_back(0.0);
        C.push_back(0.0);
    }
    if (J == 2) {
        num_proc = 4;
        next_words.insert(next_words.end(), initial_words, initial_words+4);
        last_word.insert(last_word.end(), l , l+4);
        residency_time.insert(residency_time.end(), f_zeros, f_zeros+4);
        num_replacements.insert(num_replacements.end(), i_zeros, i_zeros+4);
        page_faults.insert(page_faults.end(), i_zeros, i_zeros+4);
        float a[4]= {1.0,1.0,1.0,1.0};
        A.insert(A.end(), a, a+4);
        B.insert(B.end(), f_zeros, f_zeros+4);
        C.insert(C.end(), f_zeros, f_zeros+4);
    }
    if (J == 3) {
        num_proc = 4;
        next_words.insert(next_words.end(), initial_words, initial_words+4);
        last_word.insert(last_word.end(), l , l+4);
        residency_time.insert(residency_time.end(), f_zeros, f_zeros+4);
        num_replacements.insert(num_replacements.end(), i_zeros, i_zeros+4);
        page_faults.insert(page_faults.end(), i_zeros, i_zeros+4);
        A.insert(A.end(), f_zeros, f_zeros+4);
        B.insert(B.end(), f_zeros, f_zeros+4);
        C.insert(C.end(), f_zeros, f_zeros+4);
    }
    if (J == 4) {
        num_proc = 4;
        next_words.insert(next_words.end(), initial_words, initial_words+4);
        last_word.insert(last_word.end(), l , l+4);
        residency_time.insert(residency_time.end(), f_zeros, f_zeros+4);
        num_replacements.insert(num_replacements.end(), i_zeros, i_zeros+4);
        page_faults.insert(page_faults.end(), i_zeros, i_zeros+4);
        float a[4] = {.75,.75,.75,.5};
        float b[4]={.25,0.0,.125,.125};
        float c[4] = {0.0,0.25,0.125,0.125};
        A.insert(A.end(), a, a+4);
        B.insert(B.end(), b , b+4);
        C.insert(C.end(), c , c+4);
        
    }
    

    int cycle_num = 0;
    for  (int k = 0; k < ceil(N/3.0) ; k++) {
        cout << "Iteration: " << k << endl;
        int ouststanding_requests = N-3*k;
        int iters = min(3, ouststanding_requests);
        for (int running = 0 ; running < num_proc ; running++) {
            cout << "Process: " << running << endl;
            for (int j = 0 ; j < iters ; j++) {
                cout << "Cycle " << cycle_num << endl;
                // find next word reference
                int next_word = next_words[running];
                // update last_word
                last_word[running] = next_word;
                // compute page reference
                int page = next_word / P;
                // Check if there is a pagefault
                bool pagefault = true;
                for (int i = 0 ; i < num_frames ; i++) {
                    if (frame_table[i].present && frame_table[i].process == running && frame_table[i].page == page) {
                        pagefault = false;
                        pager->notify_referenced(i);
                        cout << "Referenced: " << i << endl;
                        break;
                    }
                }

                cout << "Pagefault: " << pagefault << endl;
                if (pagefault) {
                    page_faults[running] += 1;
                    int to_replace = -1;
                    int free_frames = 0;
                    for (int i = num_frames-1; i >= 0 ; i--) {
                        if (!frame_table[i].present && to_replace == -1) {
                            to_replace = i;
                        }
                        if (!frame_table[i].present)
                            free_frames += 1;
                    } // end for
                    cout << "Free frames: " << free_frames << endl;
                    if (to_replace != -1) 
                    cout << "Page In: " << to_replace << endl;
                    // call page replacement algorithm
                    if (to_replace==-1) {
                        to_replace = pager->find_replacement();
                        cout << "Page In: " << to_replace << endl;
                        int residency = cycle_num - frame_table[to_replace].page_in;
                        residency_time[frame_table[to_replace].process] += residency;
                        num_replacements[frame_table[to_replace].process]++;
                    }

                    pager->notify_paged_in(to_replace);
                    pager->notify_referenced(to_replace);
                    frame_table[to_replace].process = running;
                    frame_table[to_replace].page_in = cycle_num;
                    frame_table[to_replace].page = page;
                    frame_table[to_replace].present = true;
                } // end pagefault if
                next_words[running] = get_next_word(running);
                cycle_num++;
            } // end quantum loop
        }
    }   // end outer loop
    
    int total_faults = 0;
    float avg_residency = 0.0;
    int total_replacements = 0;
    for (int i = 0 ; i < num_proc ; i++) {
        total_faults += page_faults[i];
        avg_residency += residency_time[i];
        total_replacements +=  num_replacements[i];
        if (num_replacements[i] > 0)
            cout << "Process" << '\t' <<  i+1 << '\t' << "had" << '\t' << page_faults[i] << '\t' << "page faults and  an avg. residency of" << '\t' << residency_time[i] / num_replacements[i]  << endl;
        else
            cout << "Process" << '\t' <<  i+1 << '\t' << "had" << '\t' << page_faults[i] << '\t' << "page faults. With no evictions, the average residence is undefined." << endl;

    }
    if (total_replacements > 0)
        cout << "The total number of faults is " << total_faults <<  " and the overall average residency is " << avg_residency / total_replacements  << endl;
    else
        cout << "The total number of faults is " << total_faults <<  ". With no evictions, the average residence is undefined."  << endl;
    
    return 0;
}
