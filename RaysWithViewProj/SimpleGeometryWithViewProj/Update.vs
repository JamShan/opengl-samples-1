#version 450 core
precision highp float;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 prev_position;
layout(location = 2) in vec3 direction;
layout(location = 3) in vec4 color;
layout(location = 4) in uint life;
layout(location = 5) in float speed;
layout(location = 6) in float size;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 emitterPosition;
uniform uint deltaTime;
 
out vec3 out_Position;
out	vec3 out_PreviousPosition;
out vec3 out_Direction;
out vec4 out_Color;
out uint out_Life;
out float out_Speed;
out float out_Size;

const float TWO_PI = 6.28318530718;
const float UINT_MAX = 4294967295.0;

uint randhash(uint seed)
{
    uint i=(seed^12345391u)*2654435769u;
    i^=(i<<6u)^(i>>26u);
    i*=2654435769u;
    i+=(i<<5u)^(i>>12u);
    return i;
}

float randhashf(uint seed, float b)
{
    return float(b * randhash(seed)) / UINT_MAX;
}

void main() 
{
  vec3 lvPosition  = position;
  vec3 lvPreviousPosition = prev_position;
  vec3 lvDirection = direction;
  vec4 lvColor = color;
  
  uint lvLife = life;
	float lvSpeed = speed;
  float lvSize = size;

	if(lvLife != 0)
  {
		lvPreviousPosition = lvPosition;
		lvPosition += lvDirection * lvSpeed;
    lvPosition -= vec3(0.0, 0.0004, 0.0);
    
		--lvLife;
	}
  else
  { 
		uint seed = uint(deltaTime + gl_VertexID); 
    float yaw = randhashf(seed++, TWO_PI);
		float pitch = randhashf(seed++, TWO_PI);
    
    lvDirection.x = -sin(yaw) * cos(pitch);
    lvDirection.y = sin(pitch);
    lvDirection.z = cos(pitch) * cos(yaw);	 
    
		lvPosition = emitterPosition;
		lvPreviousPosition = emitterPosition;
    lvDirection *= 0.00005 + (randhashf(seed++, 0.001 ));  
    
    lvColor = vec4(randhashf(seed++,1), randhashf(seed++,1), randhashf(seed++,1), randhashf(seed++,1));
    lvLife = 480 + uint(randhashf(seed++, 250));
    lvSpeed = 0.5 + (randhashf(seed++, 1.8 ));
    lvSize = 0.5 + (randhashf(seed++, 1.7 ));
	}  

  out_Position = lvPosition;	
	out_PreviousPosition = lvPreviousPosition;		
	out_Direction = lvDirection;
  out_Color = lvColor;
  out_Life = lvLife;
  out_Speed = lvSpeed;
  out_Size = lvSize;
  
  gl_Position = projection * view * vec4(lvPosition, 1.0);
}