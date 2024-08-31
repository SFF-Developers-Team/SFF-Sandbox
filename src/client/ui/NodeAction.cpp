#include "NodeAction.hpp"

sandbox_ui::NodeAction::NodeAction() {
    pause();
    setupKeyframeArray();

    m_backend.valid = true;
}

void sandbox_ui::NodeAction::update() {
    setupKeyframeArray();

    if (!running() || paused()) return;

    if (getSpeed() == 0) {
        m_backend.delta = 0;
    } else {
        m_backend.delta /= getSpeed();
    }

    _ntRendererUpdateAnimation(getActionBackend());

    if (finished() && m_shouldCallFunc) {
        if (m_completeFunc != nullptr) m_completeFunc(this);

        m_shouldCallFunc = false;
    }
}

sandbox_ui::NodeAction::Backend *sandbox_ui::NodeAction::getActionBackend() {
    return &m_backend;
}

void sandbox_ui::NodeAction::linkAnimation(NodeAction *action, bool influence) {
    m_backend.linked_animation = action->getActionBackend();
    m_backend.influenced = influence;
}

void sandbox_ui::NodeAction::unlinkAnimation() {
    m_backend.linked_animation = nullptr;
    m_backend.influenced = false;
}

sandbox_ui::NodeAction::NodeAction(Backend anim) {
    m_backend = anim;

    if (m_backend.keyframes != nullptr) {
        m_keyframes.assign(m_backend.keyframes, m_backend.keyframes + m_backend.count);
    }

    setupKeyframeArray();

    pause();

    m_backend.valid = true;
}

void sandbox_ui::NodeAction::createKeyframe(double time, double next_value, renderer_tweak_type easing) {
    struct renderer_keyframe kf = {};
    kf.easing = easing;
    kf.length = time;
    kf.ending_value = next_value;

    m_keyframes.push_back(kf);
    
    setupKeyframeArray();
}

void sandbox_ui::NodeAction::setupKeyframeArray() {
    m_backend.keyframes = m_keyframes.data();
    m_backend.count = m_keyframes.size();
}

void sandbox_ui::NodeAction::start() {
    m_paused = false;
    m_running = true;
}
void sandbox_ui::NodeAction::pause() {
    m_paused = true;
    m_running = false;
}
void sandbox_ui::NodeAction::reset() {
    m_backend.completed = false;
    m_backend.completed_local = false;
    m_backend.current_keyframe = 0;
    m_backend.current_value = m_backend.starting_value;
    m_backend.itime = 0;
    m_backend.local_current_value = m_backend.current_value;
    m_backend.time = 0;
}

bool sandbox_ui::NodeAction::paused() {
    return m_paused;
}
bool sandbox_ui::NodeAction::running() {
    return m_running;
}

double sandbox_ui::NodeAction::getCurrentValue() {
    return m_backend.current_value;
}
double sandbox_ui::NodeAction::getCurrentValue(int animID) {
    return _ntRendererGetAnimationResult(getActionBackend(), animID);
}

bool sandbox_ui::NodeAction::animationExists(int animID) {
    return _ntRendererAnimIdExists(getActionBackend(), animID);
}

void sandbox_ui::NodeAction::setParameterToModify(const std::string &param) {
    m_modifiedParam = param;
}
std::string sandbox_ui::NodeAction::getModifiedParameter() {
    return m_modifiedParam;
}

void sandbox_ui::NodeAction::setID(int id) {
    m_backend.anim_id = id;
}
int sandbox_ui::NodeAction::getID() {
    return m_backend.anim_id;
}

bool sandbox_ui::NodeAction::finished() {
    return m_backend.completed;
}

void sandbox_ui::NodeAction::setStartingValue(double value) {
    m_backend.starting_value = value;
}

void sandbox_ui::NodeAction::setOnComplete(Callback func) {
    m_completeFunc = func;
}

void sandbox_ui::NodeAction::setSpeed(double speed) {
    m_speed = speed;
}
double sandbox_ui::NodeAction::getSpeed() {
    return m_speed;
}