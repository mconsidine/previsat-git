#ifndef THREADCALCULS_H
#define THREADCALCULS_H

#include <QThread>
#include "librairies/observateur/observateur.h"
#include "previsions/conditions.h"

class ThreadCalculs : public QThread
{
public:
    enum TypeCalcul {
        PREVISION,
        IRIDIUM,
        EVENEMENTS,
        TRANSITS
    };

    ThreadCalculs(const TypeCalcul typeCalcul, const Conditions &conditions, const Observateur &observateur);
    void run();

    TypeCalcul getTypeCalcul() const;

private:
    TypeCalcul _typeCalcul;
    Conditions _conditions;
    Observateur _observateur;
};

#endif // THREADCALCULS_H
