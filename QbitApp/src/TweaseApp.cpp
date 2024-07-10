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
    case 0:  selectedEaseFunction = Ease::linear; break;
    case 1:  selectedEaseFunction = Ease::quadIn; break;
    case 2:  selectedEaseFunction = Ease::quadOut; break;
    case 3:  selectedEaseFunction = Ease::quadInOut; break;
    case 4:  selectedEaseFunction = Ease::cubicIn; break;
    case 5:  selectedEaseFunction = Ease::cubicOut; break;
    case 6:  selectedEaseFunction = Ease::cubicInOut; break;
    case 7:  selectedEaseFunction = Ease::quartIn; break;
    case 8:  selectedEaseFunction = Ease::quartOut; break;
    case 9:  selectedEaseFunction = Ease::quartInOut; break;
    case 10: selectedEaseFunction = Ease::quintIn; break;
    case 11: selectedEaseFunction = Ease::quintOut; break;
    case 12: selectedEaseFunction = Ease::quintInOut; break;
    case 13: selectedEaseFunction = Ease::sineIn; break;
    case 14: selectedEaseFunction = Ease::sineOut; break;
    case 15: selectedEaseFunction = Ease::sineInOut; break;
    case 16: selectedEaseFunction = Ease::expoIn; break;
    case 17: selectedEaseFunction = Ease::expoOut; break;
    case 18: selectedEaseFunction = Ease::expoInOut; break;
    case 19: selectedEaseFunction = Ease::circIn; break;
    case 20: selectedEaseFunction = Ease::circOut; break;
    case 21: selectedEaseFunction = Ease::circInOut; break;
    case 22: selectedEaseFunction = Ease::backIn; break;
    case 23: selectedEaseFunction = Ease::backOut; break;
    case 24: selectedEaseFunction = Ease::backInOut; break;
    case 25: selectedEaseFunction = Ease::elasticIn; break;
    case 26: selectedEaseFunction = Ease::elasticOut; break;
    case 27: selectedEaseFunction = Ease::elasticInOut; break;
    case 28: selectedEaseFunction = Ease::bounceIn; break;
    case 29: selectedEaseFunction = Ease::bounceOut; break;
    case 30: selectedEaseFunction = Ease::bounceInOut; break;
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
