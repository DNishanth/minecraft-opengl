#include "Camera.hpp"

#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include <iostream>

Camera& Camera::Instance(){
    static Camera* instance = new Camera();
    return *instance;
}

void Camera::MouseLook(int mouseX, int mouseY){
    float sensitivity = 0.5f;
    float xDelta = (mouseX - m_oldMousePosition.x) * sensitivity;
    float yDelta = (m_oldMousePosition.y - mouseY) * sensitivity;

    yaw += xDelta;
    pitch += yDelta;
    // Restrict looking too far up and down on y axis
    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
    glm::vec3 direction = glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    );

    m_viewDirection = glm::normalize(direction);

    m_oldMousePosition.x = mouseX;
    m_oldMousePosition.y = mouseY;

    // Record our new position as a vector
    // glm::vec2 newMousePosition(mouseX, mouseY);
    // // Detect how much the mouse has moved since
    // // the last time
    // glm::vec2 mouseDelta = 0.01f*(newMousePosition-m_oldMousePosition);

    // m_viewDirection = glm::mat3(glm::rotate(-mouseDelta.x, m_upVector)) * m_viewDirection;

    // // Update our old position after we have made changes
    // m_oldMousePosition = newMousePosition;
    
}

void Camera::MoveForward(float speed){
    m_eyePosition += speed * m_viewDirection;
}

void Camera::MoveBackward(float speed){
    m_eyePosition -= speed * m_viewDirection;
}

void Camera::MoveLeft(float speed){
    m_eyePosition -= speed * glm::normalize(glm::cross(m_viewDirection, m_upVector));
}

void Camera::MoveRight(float speed){
    m_eyePosition += speed * glm::normalize(glm::cross(m_viewDirection, m_upVector));
}

void Camera::MoveUp(float speed){
    m_eyePosition.y += speed;
}

void Camera::MoveDown(float speed){
    m_eyePosition.y -= speed;
}

float Camera::GetEyeXPosition(){
    return m_eyePosition.x;
}

float Camera::GetEyeYPosition(){
    return m_eyePosition.y;
}

float Camera::GetEyeZPosition(){
    return m_eyePosition.z;
}

float Camera::GetViewXDirection(){
    return m_viewDirection.x;
}

float Camera::GetViewYDirection(){
    return m_viewDirection.y;
}

float Camera::GetViewZDirection(){
    return m_viewDirection.z;
}

Camera::Camera() {
    std::cout << "Created a Single Camera!\n";
	// Position us at the origin.
    m_eyePosition = glm::vec3(0.0f,0.0f, 0.0f);
	// Looking down along the z-axis initially.
	// Remember, this is negative because we are looking 'into' the scene.
    m_viewDirection = glm::vec3(0.0f,0.0f, -1.0f);
	// For now--our upVector always points up along the y-axis
    m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    m_oldMousePosition = glm::vec2(0.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
}

glm::mat4 Camera::GetWorldToViewmatrix() const{
    // Think about the second argument and why that is
    // setup as it is.
    return glm::lookAt( m_eyePosition,
                        m_eyePosition + m_viewDirection,
                        m_upVector);
}