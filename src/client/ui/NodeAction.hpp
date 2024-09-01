#pragma once

#include <nt5emul/renderer_animation.h>
#include <nt5emul/renderer_keyframe.h>
#include <vector>
#include <string>
#include <functional>

namespace sandbox_ui {
    class NodeAction {
    public:
        using Backend = struct ::renderer_animation;
        using KBackend = struct ::renderer_keyframe;

        using Callback = std::function<void(NodeAction*)>;
    protected:
        Backend m_backend = {};

        std::vector<KBackend> m_keyframes = {};

        void setupKeyframeArray();

        bool m_running = false;
        bool m_paused = false;

        std::string m_modifiedParam = "";

        bool m_shouldCallFunc = true;
        Callback m_completeFunc = nullptr;

        double m_speed = 1.f;
    public:
        NodeAction(Backend anim);
        NodeAction();

        void linkAnimation(NodeAction *action, bool influence);
        void unlinkAnimation();

        void update();

        Backend *getActionBackend();

        void createKeyframe(double time, double next_value, renderer_tweak_type easing = renderer_tweak_type::TOLinear);
    
        void start(); // also works as resume()
        void pause();
        void reset();

        bool paused();
        bool running();

        bool finished();

        double getCurrentValue();
        double getCurrentValue(int animID);

        bool animationExists(int animID);

        void setParameterToModify(const std::string &param);
        std::string getModifiedParameter();

        void setID(int id);
        int getID();

        void setStartingValue(double value);

        void setOnComplete(Callback func);
        
        void setSpeed(double speed);
        double getSpeed();
    };
}