#include <Qbit.h>
#include "Qbit/Core/EntryPoint.h"
#include "TweaseApp.h"

class QbitApp : public Qbit::Application
{
public:
    QbitApp(const Qbit::ApplicationSpecification& specification)
        : Qbit::Application(specification)
    {
        PushLayer(new TweaseApp());
    }

    ~QbitApp() = default;
};

Qbit::Application* Qbit::CreateApplication(Qbit::ApplicationCommandLineArgs args)
{
    Qbit::ApplicationSpecification spec;
    spec.Name = "TweaseApp";
    spec.WorkingDirectory = "../QbitApp";
    spec.CommandLineArgs = args;

    return new QbitApp(spec);
}
