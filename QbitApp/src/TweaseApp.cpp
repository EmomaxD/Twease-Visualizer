#include "qbpch.h"
#include "TweaseApp.h"

#include "Twease/Twease.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


TweaseApp::TweaseApp()
    : Layer("TweaseApp"), m_CameraController(1600.0f / 900.0f)
{
}

void TweaseApp::OnAttach()
{
    QB_PROFILE_FUNCTION();
}

void TweaseApp::OnDetach()
{
    QB_PROFILE_FUNCTION();
}


struct AnimationParams {
    glm::vec2 startPos;
    glm::vec2 endPos;
    float duration;
    float startTime;
    std::function<double(double)> easingFunc;
    std::string easingFuncName;
};

std::vector<AnimationParams> circles;

void TweaseApp::OnUpdate(Qbit::Timestep ts)
{
    QB_PROFILE_FUNCTION();

    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    Qbit::Renderer2D::ResetStats();

    {
        QB_PROFILE_SCOPE("RendererPrep");
        Qbit::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Qbit::RenderCommand::Clear();
    }

    {
        QB_PROFILE_SCOPE("Renderer - Draw");

        static float elapsedTime = 0.0f;
        elapsedTime += ts.GetSeconds();



        Qbit::Renderer2D::BeginScene(m_CameraController.GetCamera());

        for (const auto& params : circles) {
            float cycleTime = fmod(elapsedTime, 2.0f * params.duration);


            float t = cycleTime < params.duration ? cycleTime / params.duration : (2.0f * params.duration - cycleTime) / params.duration;
            double easedT = params.easingFunc(t);

            glm::vec2 currentPos = glm::mix(params.startPos, params.endPos, easedT);

            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 1.0f));

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(currentPos, 0.0f)) * scaleMatrix;

            Qbit::Renderer2D::DrawCircle(transform, glm::vec4{ 0.8f, 0.8f, 0.8f, 1.0f });
        }

        Qbit::Renderer2D::EndScene();
    }
}

static void AddCircle(std::function<double(double)> selectedEaseFunction, const char* selectedEaseFunctionName, float duration = 2.0f) {
    AnimationParams a;

    size_t n = circles.size();

    const float startX = -5.0f;
    const float endX = 5.0f;
    float y = 0.0f;
    y -= n * 1.5f;

    a.startPos = { startX, y };
    a.endPos = { endX, y };
    a.duration = duration;
    a.startTime = 1.0f;
    a.easingFunc = selectedEaseFunction;
    a.easingFuncName = selectedEaseFunctionName;

    circles.push_back(a);
}

static void RemoveCircle(size_t index) {
    if (index < circles.size()) {
        circles.erase(circles.begin() + index);
    }
}

static void ApplyDuration(size_t index, float duration) {
    if (index < circles.size()) {
        circles[index].duration = duration;
    }
}

void TweaseApp::OnImGuiRender()
{

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mainWindowPos = ImGui::GetMainViewport()->Pos;
    ImVec2 mainWindowSize = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(mainWindowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(mainWindowSize.x * 0.2f, mainWindowSize.y), ImGuiCond_Always);



    ImGui::Begin("Control Panel");

    const char* easeFunctions[] = {
        "Linear", "Quadratic In", "Quadratic Out", "Quadratic InOut",
        "Cubic In", "Cubic Out", "Cubic InOut",
        "Quartic In", "Quartic Out", "Quartic InOut",
        "Quintic In", "Quintic Out", "Quintic InOut",
        "Sine In", "Sine Out", "Sine InOut",
        "Exponential In", "Exponential Out", "Exponential InOut",
        "Circular In", "Circular Out", "Circular InOut",
        "Back In", "Back Out", "Back InOut",
        "Elastic In", "Elastic Out", "Elastic InOut",
        "Bounce In", "Bounce Out", "Bounce InOut"
    };
    static int currentEaseFunction = 0;
    std::function<double(double)> selectedEaseFunction = nullptr;
    const char* selectedEaseFunctionName = easeFunctions[currentEaseFunction];
    static float duration = 2.0f;

    // Assign the selected easing function
    switch (currentEaseFunction) {
    case 0:  selectedEaseFunction = Twease::linear; break;
    case 1:  selectedEaseFunction = Twease::quadIn; break;
    case 2:  selectedEaseFunction = Twease::quadOut; break;
    case 3:  selectedEaseFunction = Twease::quadInOut; break;
    case 4:  selectedEaseFunction = Twease::cubicIn; break;
    case 5:  selectedEaseFunction = Twease::cubicOut; break;
    case 6:  selectedEaseFunction = Twease::cubicInOut; break;
    case 7:  selectedEaseFunction = Twease::quartIn; break;
    case 8:  selectedEaseFunction = Twease::quartOut; break;
    case 9:  selectedEaseFunction = Twease::quartInOut; break;
    case 10: selectedEaseFunction = Twease::quintIn; break;
    case 11: selectedEaseFunction = Twease::quintOut; break;
    case 12: selectedEaseFunction = Twease::quintInOut; break;
    case 13: selectedEaseFunction = Twease::sineIn; break;
    case 14: selectedEaseFunction = Twease::sineOut; break;
    case 15: selectedEaseFunction = Twease::sineInOut; break;
    case 16: selectedEaseFunction = Twease::expoIn; break;
    case 17: selectedEaseFunction = Twease::expoOut; break;
    case 18: selectedEaseFunction = Twease::expoInOut; break;
    case 19: selectedEaseFunction = Twease::circIn; break;
    case 20: selectedEaseFunction = Twease::circOut; break;
    case 21: selectedEaseFunction = Twease::circInOut; break;
    case 22: selectedEaseFunction = Twease::backIn; break;
    case 23: selectedEaseFunction = Twease::backOut; break;
    case 24: selectedEaseFunction = Twease::backInOut; break;
    case 25: selectedEaseFunction = Twease::elasticIn; break;
    case 26: selectedEaseFunction = Twease::elasticOut; break;
    case 27: selectedEaseFunction = Twease::elasticInOut; break;
    case 28: selectedEaseFunction = Twease::bounceIn; break;
    case 29: selectedEaseFunction = Twease::bounceOut; break;
    case 30: selectedEaseFunction = Twease::bounceInOut; break;
    }

    if (ImGui::Button("Add Circle")) {

        AddCircle(selectedEaseFunction, selectedEaseFunctionName, duration);

        currentEaseFunction = 0;
    }

    ImGui::Combo("Easing Function", &currentEaseFunction, easeFunctions, IM_ARRAYSIZE(easeFunctions));

    ImGui::SliderFloat("Duration", &duration, 0.1f, 10.0f, "%.1f s");

    if (ImGui::Button("Apply Duration For Each")) {
        for (auto& circle : circles) {
            circle.duration = duration;
        }
    }

    ImGui::Separator();
    ImGui::Text("Circles:");

    for (size_t i = 0; i < circles.size(); ++i) {
        ImGui::PushID(static_cast<int>(i));

        ImGui::Text("Circle %zu (%s)", i, circles[i].easingFuncName.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Remove")) {
            RemoveCircle(i);
        }
        ImGui::SameLine();
        if (ImGui::Button("Apply Duration")) {
            ApplyDuration(i, duration);
        }

        ImGui::PopID();
    }


    ImGui::End();
}

void TweaseApp::OnEvent(Qbit::Event& e)
{
    m_CameraController.OnEvent(e);
}
