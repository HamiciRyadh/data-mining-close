//
// Created by hamici on 22/07/2019.
//

#include "close.h"

void closeAlgorithm(Matrix matrix, double minSup) {
    ListContainer *listContainer = NULL, *listContainerPrev = NULL;
    ListListContainer *head = NULL, *previous = NULL, *temp = NULL;
    double begin, end, sum = 0;

    printf("Starting Gen1\n");
    begin = clock();
    ListItem *items = findGen1SupportAndClosure(&listContainer, matrix, minSup);
    end = clock();
    if (listContainer == NULL) {
        printf("Gen1 time : %lfs.\n", (end - begin)/CLOCKS_PER_SEC);
        return;
    }

    items = sortListItem(items);
    printf("Gen1 time : %lfs.\n", (end - begin)/CLOCKS_PER_SEC);
    sum += (end - begin);

    head = (ListListContainer*)malloc(sizeof(ListListContainer));
    head->listContainer = listContainer;
    head->next = NULL;
    previous = head;

    int i = 2;
    do {
        printf("Beginning Gen%d\n", i);
        begin = clock();
        listContainer = findNextGenerators(listContainer);
        if (listContainer == NULL) {
            printf("Gen%d time : %lfs.\n", i, (end - begin)/CLOCKS_PER_SEC);
            sum += (end - begin);
            break;
        }

        ListContainer *iterator = listContainer;
        listContainerPrev = NULL;
        while (iterator != NULL) {
            findSupportAndClosure(&iterator->container, matrix, minSup, items);
            if (iterator->container.support < minSup) {
                if (listContainerPrev == NULL) {
                    listContainer = listContainer->next;
                    free(iterator);
                    iterator = listContainer;
                } else {
                    listContainerPrev->next = iterator->next;
                    free(iterator);
                    iterator = listContainerPrev->next;
                }
            } else {
                listContainerPrev = iterator;
                iterator = iterator->next;
            }
        }
        end = clock();
        printf("Gen%d time : %lfs.\n", i, (end - begin)/CLOCKS_PER_SEC);
        sum += (end - begin);
        i++;

        temp = (ListListContainer*)malloc(sizeof(ListListContainer));
        temp->listContainer = listContainer;
        temp->next = NULL;
        previous->next = temp;
        previous = temp;
    } while (listContainer != NULL);

    ListListContainer *tmp = head;
    while (tmp != NULL) {
        printListContainer(tmp->listContainer);
        tmp = tmp->next;
    }
    printRulesInFile(findRules(head));

    printf("Total time : %lfs.\n", sum/CLOCKS_PER_SEC);
}

ListContainer *findNextGenerators(ListContainer *currentGeneration) {
    ListContainer *head1 = currentGeneration, *head2 = NULL, *newList = NULL, *previous = NULL, *temp = NULL;
    Itemset *generator = NULL;

    if (head1 == NULL || head1->next == NULL) return NULL;

    while (head1 != NULL) {
        head2 = head1->next;
        while (head2 != NULL) {
            generator = mergeGenerators(head1->container.generator, head2->container.generator);
            //If NULL : The generators cannot be merged, thus it is the case for all future values of head2.
            if (generator == NULL) {
                break;
            }
            if (isContained(head1->container.closure, generator) == 0 && isContained(head2->container.closure, generator) == 0) {
                //Not included in the closures, it is a valid generator.
                temp = (ListContainer*)malloc(sizeof(ListContainer));
                temp->container.generator = generator;
                temp->container.closure = NULL;
                temp->next = NULL;

                if (previous == NULL) newList = temp;
                else previous->next = temp;
                previous = temp;
            }

            head2 = head2->next;
        }

        head1 = head1->next;
    }

    return newList;
}

ListRule *findRules(ListListContainer *listListContainer) {
    ListRule *listRule = NULL, *temp = NULL, *prec = NULL;
    int count = 0;
    ListListContainer *head = listListContainer;
    ListContainer *listContainer;
    Itemset *generator, *closure, *ruleHead = NULL, *rulePrec = NULL, *ruleTemp;
    while (head != NULL) {
        listContainer = head->listContainer;

        while (listContainer != NULL) {
            ruleHead = NULL;
            rulePrec = NULL;
            generator = listContainer->container.generator;
            closure = listContainer->container.closure;
            while (generator != NULL) {
                if (closure->item == generator->item) {
                    generator = generator->nextItem;
                    closure = closure->nextItem;
                } else {
                    ruleTemp = closure;
                    closure = closure->nextItem;

                    if (rulePrec == NULL) ruleHead = ruleTemp;
                    else rulePrec->nextItem = ruleTemp;
                    rulePrec = ruleTemp;
                }
            }
            while (closure != NULL) {
                ruleTemp = closure;
                closure = closure->nextItem;

                if (rulePrec == NULL) ruleHead = ruleTemp;
                else rulePrec->nextItem = ruleTemp;
                rulePrec = ruleTemp;
            }

            if (ruleHead != NULL) {
                temp = (ListRule*)malloc(sizeof(ListRule));
                temp->rule.antecedant = listContainer->container.generator;
                temp->rule.succedant = ruleHead;
                temp->next = NULL;
                count++;
                if (prec == NULL) {
                    listRule = temp;
                }
                else {
                    prec->next = temp;
                }
                prec = temp;
            }

            listContainer = listContainer->next;
        }
        head = head->next;
    }
    printf("Count : %d\n", count);
    return listRule;
}

Itemset *mergeGenerators(Itemset *generator1, Itemset *generator2) {
    Itemset *merged = NULL, *temp = NULL, *previous = NULL, *iterator1 = generator1, *iterator2 = generator2;

    while (iterator1->nextItem != NULL && iterator2->nextItem != NULL) {
        if (iterator1->item != iterator2->item) return NULL;

        iterator1 = iterator1->nextItem;
        iterator2 = iterator2->nextItem;
    }

    iterator1 = generator1;
    while (iterator1->nextItem != NULL) {
        temp = (Itemset*)malloc(sizeof(Itemset));
        temp->item = iterator1->item;
        temp->nextItem = NULL;

        if (previous == NULL) {
            merged = temp;
        } else {
            previous->nextItem = temp;
        }
        previous = temp;

        iterator1 = iterator1->nextItem;
    }

    int first, second;
    if (iterator1->item < iterator2->item){
        first = iterator1->item;
        second = iterator2->item;
    } else {
        first = iterator2->item;
        second = iterator1->item;
    }
    temp = (Itemset*)malloc(sizeof(Itemset));
    temp->item = first;
    if (previous == NULL) {
        merged = temp;
    } else {
        previous->nextItem = temp;
    }
    previous = temp;
    temp = (Itemset*)malloc(sizeof(Itemset));
    temp->item = second;
    temp->nextItem = NULL;
    previous->nextItem = temp;

    return merged;
}

unsigned char isContained(Itemset *container, Itemset *contained) {
    Itemset *iterator1 = contained, *iterator2 = container;

//    printf("Contained : ");
//    while (iterator1 != NULL) {
//        printf("%d ", iterator1->item);
//        iterator1 = iterator1->nextItem;
//    }
//    iterator1 = contained;
//    printf("\nContainer : ");
//    while (iterator2 != NULL) {
//        printf("%d ", iterator2->item);
//        iterator2 = iterator2->nextItem;
//    }
//    iterator2 = container;
//    printf("\n");
    while (iterator1 != NULL && iterator2 != NULL) {
        if (iterator1->item == iterator2->item) {
            iterator1 = iterator1->nextItem;
        }
        iterator2 = iterator2->nextItem;
    }
//    printf("Result : %d\n\n", (iterator1 == NULL) ? 1 : 0);

    return (iterator1 == NULL) ? 1 : 0;
}

void findSupportAndClosure(Container *container, Matrix matrix, double minSup, ListItem *sortedItems) {
    ListItem *sortedGenerator = findAssociatedListItem(container->generator, sortedItems), *iterator = NULL;

    int found = 0, count = 0, i = 0, j;

    while (i < matrix.rows && found == 0) {
        found = 1;
        iterator = sortedGenerator;
        while (iterator != NULL) {
            if ((matrix.bitmap[i][iterator->item.position.cell] & iterator->item.position.bit) == 0) {
                found = 0;
                break;
            }
            iterator = iterator->next;
        }
        i++;
    }
    if (found == 0 || i > matrix.rows) return;

    uint8_t *row = (uint8_t*)malloc(matrix.cols* sizeof(uint8_t));
    memcpy(row, matrix.bitmap[i-1], matrix.cols* sizeof(uint8_t));
    count++;
    for (; i < matrix.rows; i++) {
        found = 1;
        iterator = sortedGenerator;
        while (iterator != NULL) {
            if ((matrix.bitmap[i][iterator->item.position.cell] & iterator->item.position.bit) == 0) {
                found = 0;
                break;
            }
            iterator = iterator->next;
        }
        if (found == 1) {
            count++;
            for (j = 0; j < matrix.cols; j++) {
                row[j] = row[j] & matrix.bitmap[i][j];
            }
        }
    }
    container->support = (double)count/(double)matrix.rows;

    if (container->support >= minSup) {
        //Creating the closure.
        Itemset *previousItemset = NULL, *currentItemset = NULL;
        BitmapPosition position;
        for (j = 1; j <= matrix.variables; j++) {
            position = findBitmapPosition(j);
            if ((row[position.cell] & position.bit) != 0) {
                //The item is part of the closure.
                currentItemset = (Itemset*)malloc(sizeof(Itemset));
                currentItemset->item = j;
                currentItemset->nextItem = NULL;
                if (previousItemset == NULL) {
                    container->closure = currentItemset;
                    previousItemset = currentItemset;
                } else {
                    previousItemset->nextItem = currentItemset;
                    previousItemset = currentItemset;
                }
            }
        }
    }
    free(sortedGenerator);
    free(row);
}

ListItem *findGen1SupportAndClosure(ListContainer **pListContainer, Matrix matrix, double minSup) {
    ListItem *basicItems = NULL, *lastItem = NULL, *temp = NULL;
    uint8_t *row = (uint8_t*)malloc(matrix.cols* sizeof(uint8_t));

    ListContainer *head = NULL, *currentElement = NULL;
    Itemset *generator = NULL, *closure = NULL, *previousItemset = NULL, *currentItemset = NULL;

    int i, j, count;
    for (int var = 1; var <= matrix.variables; var++) {
        i = 0;
        count = 0;
        Item currentItem;
        currentItem.index = var;
        currentItem.position = findBitmapPosition(var);
//        printf("Var : %d, Cell : %d, Bit : %d\n", var, currentItem.position.cell, currentItem.position.bit);
        //We position ourselves on the first occurrence of the item.
        while (i < matrix.rows && (matrix.bitmap[i][currentItem.position.cell] & currentItem.position.bit) == 0) {
            i++;
        }
        currentItem.firstOcc = i;

        if (i < matrix.rows) {
            memcpy(row, matrix.bitmap[i], matrix.cols* sizeof(uint8_t));
            i++;
            count++;
        }
        for (; i < matrix.rows; i++) {
//            int val = matrix.bitmap[i][currentItem.position.cell] & currentItem.position.bit;
//            printf("Row : %d, Element : %d, bit : %d, result : %d\n", i, matrix.bitmap[i][currentItem.position.cell], currentItem.position.bit, val);
            if ((matrix.bitmap[i][currentItem.position.cell] & currentItem.position.bit) != 0) {
                count++;
                for (j = 0; j < matrix.cols; j++) row[j] = row[j] & matrix.bitmap[i][j];
            }
        }
        currentItem.support = (double)count/(double)matrix.rows;

        if (currentItem.support >= minSup) {
            //Adding the item to the list, because it is frequent.
            temp = (ListItem*)malloc(sizeof(ListItem));
            temp->item = currentItem;
            temp->next = NULL;

            if (basicItems == NULL) {
                basicItems = temp;
                lastItem = temp;
            } else {
                lastItem->next = temp;
                lastItem = temp;
            }

             //Creating the associated generator.
            generator = (Itemset*)malloc(sizeof(Itemset));
            generator->item = var;
            generator->nextItem = NULL;

            //Creating the closure.
            previousItemset = NULL;
            BitmapPosition position;
            for (j = 1; j <= matrix.variables; j++) {
                position = findBitmapPosition(j);
                if ((row[position.cell] & position.bit) != 0) {
                    //The item is part of the closure.
                    currentItemset = (Itemset*)malloc(sizeof(Itemset));
                    currentItemset->item = j;
                    currentItemset->nextItem = NULL;
                    if (previousItemset == NULL) {
                        closure = currentItemset;
                        previousItemset = currentItemset;
                    } else {
                        previousItemset->nextItem = currentItemset;
                        previousItemset = currentItemset;
                    }
                }
            }

            currentElement = (ListContainer*)malloc(sizeof(ListContainer));
            currentElement->container.generator = generator;
            currentElement->container.support = currentItem.support;
            currentElement->container.closure = closure;
            currentElement->next = NULL;

            if (*pListContainer == NULL) {
                *pListContainer = currentElement;
                head = currentElement;
            } else {
                head->next = currentElement;
                head = currentElement;
            }
        }
    }
    free(row);
    return basicItems;
}

//Sorts an Itemset according to the support of its items (ascending)
Itemset *sortItemset(Itemset *head, ListItem *items) {
    Itemset *newHead = NULL, *temp = NULL, *iterator = NULL, *previous = NULL;
    ListItem *itemHead;

    itemHead = items;
    while (itemHead != NULL) {

        iterator = head;
        while (iterator != NULL && iterator->item != itemHead->item.index) {
            iterator = iterator->nextItem;
        }

        if (iterator != NULL) {
            temp = (Itemset *) malloc(sizeof(Itemset));
            temp->item = iterator->item;
            temp->nextItem = NULL;

            if (previous == NULL) {
                newHead = temp;
            } else {
                previous->nextItem = temp;
            }
            previous = temp;
        }

        itemHead = itemHead->next;
    }

    return newHead;
}

ListItem *findAssociatedListItem(Itemset *head, ListItem *items) {
    ListItem *itemIterator, *sortedList = NULL, *temp = NULL, *previous = NULL;
    Itemset *itemsetIterator = head;

    while (itemsetIterator != NULL) {
        itemIterator = items;
        while (itemIterator != NULL && itemIterator->item.index != itemsetIterator->item) {
            itemIterator = itemIterator->next;
        }

        temp = (ListItem*)malloc(sizeof(ListItem));
        temp->item = itemIterator->item;
        temp->next = NULL;

        if (previous == NULL) sortedList = temp;
        else previous->next = temp;
        previous = temp;

        itemsetIterator = itemsetIterator->nextItem;
    }

    return sortedList;
}

//Sorts a ListItem according to the support of its items (ascending)
ListItem *sortListItem(ListItem *items) {
    ListItem *newHead = NULL, *temp = NULL, *iterator = NULL, *previous = NULL;
    newHead = items;
    items = items->next;
    newHead->next = NULL;

    while (items != NULL) {
        temp = items;
        items = items->next;
        if (newHead->item.support >= temp->item.support) {
            temp->next = newHead;
            newHead = temp;
        } else {
            previous = NULL;
            iterator = newHead;
            while (iterator->next != NULL && iterator->item.support < temp->item.support) {
                previous = iterator;
                iterator = iterator->next;
            }
            if (previous == NULL) {
                //If we did not enter the loop (list only contains 1 element)
                iterator->next = temp;
                temp->next = NULL;
            } else if (iterator->item.support >= temp->item.support) {
                //If we left the loop because of the 2nd condition
                previous->next = temp;
                temp->next = iterator;
            } else {
                //If we left the loop because of the 1st condition
                iterator->next = temp;
                temp->next = NULL;
            }
        }
    }
    return newHead;
}

void printListContainer(ListContainer *listContainer) {
    ListContainer *head = listContainer;
    while (head != NULL) {
        Itemset *itemset = head->container.generator;
        printf("Generator : ");
        while (itemset != NULL) {
            printf("%d ", itemset->item);
            itemset = itemset->nextItem;
        }
        printf(" , Support : %lf ", head->container.support);

        itemset = head->container.closure;
        printf(", Closure : ");
        while (itemset != NULL) {
            printf("%d ", itemset->item);
            itemset = itemset->nextItem;
        }
        printf("\n");

        head = head->next;
    }
}

void printListItem(ListItem *listItem) {
    ListItem *head = listItem;
    while (head != NULL) {
        printf("Item : %d, FirstOcc : %d, Support : %lf.\n", (head->item).index, (head->item).firstOcc, (head->item).support);
        head = head->next;
    }
}

void printRulesInFile(ListRule *rules) {
    FILE *file = fopen("rules.txt", "w");
    Itemset *left, *right;
    if (file == NULL) {
        printf("An error occurred while creating rules.txt.\n");
        exit(1);
    }
    ListRule *head = rules;
    while (head != NULL) {
        left = head->rule.antecedant;
        right = head->rule.succedant;

        while (left != NULL) {
            fprintf(file, "%d ", left->item);
            left = left->nextItem;
        }

        fprintf(file, " => ");
        while (right != NULL) {
            fprintf(file, "%d ", right->item);
            right = right->nextItem;
        }
        fprintf(file, "\n");

        head = head->next;
    }
    fclose(file);
}