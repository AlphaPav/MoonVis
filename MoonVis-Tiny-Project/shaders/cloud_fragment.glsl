#version 120
uniform float time;
uniform vec2 resolution;

float iTime = 0.0;
vec3  iResolution = vec3(0.0);
void mainImage (out vec4 fragColor, in vec2 fragCoord);

const float cloudscale = 1.1;
const float speed = 0.03*0.5;
const float clouddark = 0.5;
const float cloudlight = 0.3;
const float cloudcover = 0.2;
const float cloudalpha = 8.0;
const float skytint = 0.5;
const vec3 skycolour1 = vec3(0.4, 0.3, 0.4);
const mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );

void main(void) {
    iTime = time; //当前时间 递增
    iResolution = vec3(resolution, 0.0); //屏幕分辨率
    mainImage(gl_FragColor, gl_FragCoord.xy);
}


vec2 hash( vec2 p ) { //产生随机值
	p = vec2(dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)));
	return -1.0 + 2.0*fract(sin(p)*43758.5453123); //fract只返回小数部分
}


float noise( in vec2 p ) {
  //Uses the rand function to generate noise
    const float K1 = 0.366025404; // n=3 (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // n=3 (3-sqrt(3))/6;
	// 变换到新网格的(0, 0)点
	vec2 i = floor(p + (p.x+p.y)*K1);
	// 换算到旧网格点 i - (i.x+i.y)*K2
    vec2 a = p - i + (i.x+i.y)*K2;
    vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0); 
    // 新网格(1.0, 0.0)或(0.0, 1.0)
	vec2 b = a - o + K2;
	vec2 c = a - 1.0 + 2.0*K2;
	// 计算每个顶点的权重向量，r^2 = 0.5
    vec3 h = max(0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	// 每个顶点的梯度向量和距离向量的点乘 然后再乘上权重向量
	vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    // 之所以乘上70 是在计算了n每个分量的和的最大值以后得出的
	// 这样才能保证将n各个分量相加以后的结果在[-1, 1]之间
	return dot(n, vec3(70.0));	
}

float fbm(vec2 n) {
    //fbm stands for "Fractal Brownian Motion" 分形布朗运动模型
	float total = 0.0, amplitude = 0.1, grain =0.4;
	for (int i = 0; i < 7; i++) { //loop of octaves
		total += noise(n) * amplitude; //n 表示frequency
		n = m * n; //连续升高frequency
		amplitude *=grain; //降低amplitude
	}
	return total;
}

// -----------------------------------------------

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 p = fragCoord.xy / iResolution.xy; //根据屏幕分辨率换算到[(0.,0.),(1.,1.)]
	vec2 uv = p*vec2(iResolution.x/iResolution.y,1.0); // 根据屏幕纵横比变换
    float time = iTime * speed; // 当前增长的时间
    float q = fbm(uv * cloudscale * 0.5);
    
    //ridged noise shape
	float r = 0.0;
	uv *= cloudscale;
    uv -= q - time;
    float weight = 0.8;
    for (int i=0; i<8; i++){  //fbm的思想
		r += abs(weight*noise( uv ));
        uv = m*uv + time;
		weight *= 0.7;
    }
    
    //noise shape  
	float f = 0.0;
    uv = p*vec2(iResolution.x/iResolution.y,1.0);
	uv *= cloudscale;
    uv -= q - time;
    weight = 0.7;
    for (int i=0; i<8; i++){  //fbm的思想
		f += weight*noise( uv ); //simplex_noise
        uv = m*uv + time;
		weight *= 0.6;
    }
    
    f *= r + f;
    
    //noise colour
    float c = 0.0;
    time = iTime * speed * 2.0;
    uv = p*vec2(iResolution.x/iResolution.y,1.0);
	uv *= cloudscale*2.0;
    uv -= q - time;
    weight = 0.4;
    for (int i=0; i<7; i++){  //fbm的思想
		c += weight*noise( uv );
        uv = m*uv + time;
		weight *= 0.6;
    }
    
    //noise ridge colour
    float c1 = 0.0;
    time = iTime * speed * 3.0;
    uv = p*vec2(iResolution.x/iResolution.y,1.0);
	uv *= cloudscale*3.0;
    uv -= q - time;
    weight = 0.4;
    for (int i=0; i<7; i++){ //fbm的思想
		c1 += abs(weight*noise( uv ));
        uv = m*uv + time;
		weight *= 0.6;
    }
	
    c += c1;
    
    vec3 skycolour = mix(vec3(1.0,0.0,0.0), skycolour1, p.y);
    vec3 cloudcolour = vec3(1.1, 1.1, 0.9) * clamp((clouddark + cloudlight*c), 0.0, 1.0);
   
    f = cloudcover + cloudalpha*f*r; //调整透明度
    //天空背景色和云的颜色混合
    vec3 result = mix(skycolour, clamp(skytint * skycolour + cloudcolour, 0.0, 1.0), clamp(f + c, 0.0, 1.0));
    
	fragColor = vec4( result, 1.0 );
}