

#ifndef ___RStack_cpp__
#define ___RStack_cpp__
#include<stdio.h>
template<typename XXXXX> class RStack{
private:
    XXXXX *inner_data;
    RStack *N;
public:
    RStack(XXXXX *data, RStack *next = NULL) : inner_data(data), N(next){
    }
    static void push(RStack **head, XXXXX *data){
        RStack *new_head = new RStack(data, *head);
        *head = new_head;
    }
    static XXXXX* pop(RStack **head){
        if (*head == NULL) return NULL;
        RStack *old_head = *head;
        XXXXX *data = old_head->inner_data;
        *head = old_head->N;
        delete old_head;
        return data;
    }
    static void clear(RStack **head){
        RStack *prev, *curr = *head;
        while(curr != NULL){
            prev = curr;
            curr = curr->N;
            delete prev->inner_data;
            delete prev;
        }
        *head = NULL;
    }
};

#endif
