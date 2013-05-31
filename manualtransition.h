#include <QtCore/QAbstractTransition>

#define ManualTransitionEventType (QEvent::Type)(QEvent::User + 44)

class ManualTransition : public QAbstractTransition
{
public:
    ManualTransition(QAbstractState *target)
    {
        setTargetState(target);
    }

    static void trigger(QStateMachine *machine)
    {
        machine->postEvent(new QEvent(ManualTransitionEventType));
    }

protected:

    virtual bool eventTest(QEvent *event)
    {
        return (event->type() == ManualTransitionEventType);
    }

    virtual void onTransition(QEvent *){}
};

