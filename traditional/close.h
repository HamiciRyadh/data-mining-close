//
// Created by hamici on 22/07/2019.
//

#ifndef DATA_MINING_CLOSE_CLOSE_H
#define DATA_MINING_CLOSE_CLOSE_H

#include "binary_conversion.h"

typedef struct Itemset {
    int item;
    struct Itemset *nextItem;
} Itemset;

typedef struct Container {
    Itemset *generator;
    double support;
    Itemset *closure;
} Container;

typedef struct ListContainer {
    Container container;
    struct ListContainer *next;
} ListContainer;

typedef struct ListListContainer {
    ListContainer *listContainer;
    struct ListListContainer *next;
} ListListContainer;

typedef struct Item {
    int index;
    int firstOcc;
    double support;
    BitmapPosition position;
} Item;

typedef struct ListItem {
    Item item;
    struct ListItem *next;
} ListItem;

typedef struct Rule {
    Itemset *antecedant;
    Itemset *succedant;
} Rule;

typedef struct ListRule {
    Rule rule;
    struct ListRule *next;
} ListRule;

void closeAlgorithm(Matrix matrix, double minSup);
ListContainer *findNextGenerators(ListContainer *currentGeneration);
void findSupportAndClosure(Container *container, Matrix matrix, double minSup, ListItem *sortedItems);
ListItem *findGen1SupportAndClosure(ListContainer **pListContainer, Matrix matrix, double minSup);
ListRule *findRules(ListListContainer *listListContainer);
Itemset *mergeGenerators(Itemset *generator1, Itemset *generator2);
unsigned char isContained(Itemset *container, Itemset *contained);

void printListContainer(ListContainer *listContainer);
void printListItem(ListItem *listItem);
void printRulesInFile(ListRule *rules);

ListItem *sortListItem(ListItem *items);
Itemset *sortItemset(Itemset *head, ListItem *items);
ListItem *findAssociatedListItem(Itemset *head, ListItem *items);

#endif //DATA_MINING_CLOSE_CLOSE_H
