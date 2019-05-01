//
// Created by giacomo on 16/04/19.
//

#ifndef VARSORTER_JAVA_UTILS_H
#define VARSORTER_JAVA_UTILS_H

#define ABSTRACT_CLASS              class
#define ABSTRACT                    virtual

template <typename typeComparing> using Comparator = int (*) (typeComparing *, typeComparing *);

typedef Comparator<int> IntComparator;

#endif //VARSORTER_JAVA_UTILS_H
