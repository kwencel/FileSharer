#ifndef FILESHARER_OBSERVER_H
#define FILESHARER_OBSERVER_H


class Observer {
    public:
        virtual void update(class Connection *) = 0;
};



#endif //FILESHARER_OBSERVER_H
