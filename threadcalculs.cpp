#include "threadcalculs.h"
#include "previsions/evenements.h"
#include "previsions/iridium.h"
#include "previsions/prevision.h"
#include "previsions/transitiss.h"

ThreadCalculs::ThreadCalculs(const TypeCalcul typeCalcul, const Conditions &conditions, const Observateur &observateur)
{
    _typeCalcul = typeCalcul;
    _conditions = conditions;
    _observateur = observateur;
}

void ThreadCalculs::run()
{
    switch (_typeCalcul) {
    case PREVISION:
        Prevision::CalculPassages(_conditions, _observateur);
        break;

    case IRIDIUM:
        Iridium::CalculFlashsIridium(_conditions, _observateur);
        break;

    case EVENEMENTS:
        Evenements::CalculEvenements(_conditions);
        break;

    case TRANSITS:
        TransitISS::CalculTransitsISS(_conditions, _observateur);

    default:
        break;
    }
}

ThreadCalculs::TypeCalcul ThreadCalculs::getTypeCalcul() const
{
    return _typeCalcul;
}
