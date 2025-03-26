// terrain.frag
#version 330 core
out vec4 FragColor;
in vec3 FragPos;
uniform bool underground;
uniform vec3 undergroundColor;
void main(){
    if(underground){
        FragColor = vec4(undergroundColor, 1.0);
        return;
    }
    float h = FragPos.y;
    vec3 color;
    if(h < 2.0)
        color = vec3(0.1, 0.5, 0.1);
    else if(h < 10.0)
        color = vec3(0.3, 0.3, 0.3);
    else
        color = vec3(1.0, 1.0, 1.0);
    FragColor = vec4(color, 1.0);
}
