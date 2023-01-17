varying vec3 v_world_position; 
uniform vec3 u_camera_position; 
uniform mat4 u_world_2_local; 
uniform sampler3D u_texture; 
uniform float u_ray_step;
uniform float u_brightness;  
uniform float u_classification_picker;  

// jittering
uniform bool u_jittering; 
uniform sampler2D u_jittering_table; 
uniform float u_texture_width;  
uniform bool u_jitter_function;

// Transfer Function 
uniform sampler2D u_transfer_function; 

uniform float u_current_transfer;
uniform sampler2D u_transfer;

uniform float u_transfer_max_red;
uniform float u_transfer_min_red;
uniform float u_transfer_max_green;
uniform float u_transfer_min_green;
uniform float u_transfer_max_blue;
uniform float u_transfer_min_blue;

// Clipping
uniform vec4 u_clipping_plane;




vec3 world2local(vec3 world) {
	return (u_world_2_local * vec4( world, 1.0) ).xyz;
}

vec3 local2tex(vec3 local) {
	return local * 0.5f + vec3(0.5f);
}

vec3 world2tex(vec3 world) {
	return local2tex(world2local(world));
}

float rand(vec2 co)
{
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(){
	
	int MAX_ITERATIONS = 1000;

	// Jittering 
	float offset = 0.f; 
	if (u_jittering) {
		vec2 jittering_cords = gl_FragCoord.xy / u_texture_width; 
		offset = texture2D(u_jittering_table, jittering_cords).x; 
	}
	else if (u_jitter_function)
		offset= rand(gl_FragCoord.xy);

	// 1. Ray setup
	vec3 ray_dir = normalize(v_world_position - u_camera_position);
	vec3 sample_pos = v_world_position + ray_dir * offset * u_ray_step; 
	vec4 final_color = vec4(0.f);
	

	// Ray loop
	for (int i=0; i<MAX_ITERATIONS; i++) {
		
		// Clipping 	
		float inFront = dot(u_clipping_plane, vec4(sample_pos, 1.f)); 
		if (inFront > 0) {
			sample_pos += u_ray_step * ray_dir; 
			continue; 
		}

		// 2. Volume sampling
		// convert from world to tex coordinates
		vec3 tex_cords = world2tex(sample_pos);
		// sample volume 
		float d  = texture3D(u_texture, tex_cords).x; 

		// 3. Classification
		vec4 sample_color = vec4(d,d,d,d);

		if (u_current_transfer == 1)
			{
			// Use transfer function texture for classification
			sample_color = texture(u_transfer_function, vec2(d, 0));
			}
		else if (u_current_transfer == 3)
		{
			// Use values from UI sliders for Classification
			if (u_transfer_min_red < d && d < u_transfer_max_red)
				sample_color = vec4(1,0,0,1);
			else if (u_transfer_min_blue < d && d < u_transfer_max_blue)
				sample_color = vec4(0,0,1,0.5);
			else if (u_transfer_min_green < d && d < u_transfer_max_green)
				sample_color = vec4(0,1,0,0.5);
			else
				sample_color = vec4(0.0);
		}

		else 
		{
			// Default "white" Classification purely based on the density
			sample_color.rgb *= sample_color.a;		
		}

		sample_color *= u_brightness;
		sample_color.rgb *= sample_color.a;		
	

		// 4. Composition
		final_color += u_ray_step * (1.f - final_color.a) * sample_color;

		// 5. Next sample
		sample_pos += u_ray_step * ray_dir; 

		// 6. Early termination
		//due to out of bounds 
		vec3 next_local = world2local(sample_pos);
		bvec3 greater = greaterThan(next_local, vec3(-1.001f,-1.001f,-1.001f));
		bvec3 lower = lessThan(next_local, vec3(1.001f, 1.001f, 1.001f));
		bool inside = (greater == bvec3(true, true, true)) && (lower == bvec3(true, true, true));

		if (!inside) {
			break;
		}

		//due to "full alpha"
		if (final_color.a > 1.f) {
			break;
		}
	}

	//7. Final color
	gl_FragColor = final_color ;
}