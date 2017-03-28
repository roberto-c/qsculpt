

//r^2 = x^2 + y^2;

int function1(int x, int y, float t) {
//    int dx = x + native_sin(t*0.1f)*50 - 127;
//    int dy = y + native_cos(t*0.1f)*50 - 60;
    int dx = x - 127;
    int dy = y - 70;
    return -1000 + abs((int)(native_sin(t*0.1f)*dx*dx + native_cos(t*0.1f)*dy*dy + dx*dy/(t*t)));
}

int function2(int x, int y, float t) {
    int dx = x - 127;
    int dy = y - 127;
    int dx2 = x - 200;
    int dy2 = y - 200;
    return -1000 + dy2*dy2 + dx2*dx2 - t*dx2*dx - t*t*dy2;
}

int2 function3(int x, int y, float step, float4 c) {
    if (c.x < 0.4f && c.y < 0.3f) {
        return (int2)(x, y+1);
    }
    return (int2)(x, y);
}

int function4(int x, int y, float step) {
    return 127 - y - step * 0.2f;
}

int2 function5(int x, int y, float step) {
    int2 z;
    x = x - step;
    y = y - step;
    z.x = x*x + y*y - 1000;
    z.y = y*y + 2*x*y - 1000;
    return z;
}

float squarefn(int2 p, float a, float b, float c) {
    return (float)(a*p.x*p.x + b*p.x + c);
}

kernel void filter_img(
                       read_only image2d_t src,
                       write_only image2d_t dest,
                       float step)
{
	int x = get_global_id(0);
	int y = get_global_id(1);
	
    int2 dim = get_image_dim(dest);
    int2 dimh = dim/2;
    
	if (x >= dim.x) return;
	if (y >= dim.y) return;
    
    //step = step - ((int)(step)/(300))*300;
    
    int2 p = (int2)(x, y);
    float4 c2 = read_imagef(src, p) * 0.5f; // (float4)(0,0,0, 1.0f);
    int v1 = function1(x,y,step);
    if ( v1 < 0 ) {
        float factor = clamp(((float)(-v1)/10000.0f), 0.0f, 1.0f);
        c2 += (float4)(0.1f,0.3f,1.0f, 1.0f) * factor;
    }
    int v2 = function2(x,y,step);
    if (v2 < 0 ) {
        float factor = clamp(((float)(-v2)/1000000.0f), 0.0f, 1.0f);
        c2 += (float4)(0.3f,0.2f,0.4f, 1.0f) * factor;
    }
    
    if (v1 - v2 < 0) {
        float factor = clamp(((float)(v2-v1)/1000000.0f), 0.0f, 1.0f);
        c2 += (float4)(0.7f,0.2f,0.4f, 1.0f) * factor;
    }
    
    c2 = clamp(c2, (float4)(0,0,0,1), (float4)(1,1,1,1));
    //c2 = (float4)(0, 0, 1.0f, 1.0f);
    write_imagef(dest, p, c2);
}

kernel void filter2_img(
                       read_only image2d_t src,
                       write_only image2d_t dest,
                       float step)
{
	int x = get_global_id(0);
	int y = get_global_id(1);
	
    int2 dim = get_image_dim(dest);
    int2 dimh = dim/2;
    
	if (x >= dim.x) return;
	if (y >= dim.y) return;
    
    //step = step - ((int)(step)/(300))*300;
    
    int2 p = (int2)(x, y);
    float4 c2 = read_imagef(src, p); // (float4)(0,0,0, 1.0f);
    float y1 = squarefn(p, 2, 5, -100);
    float factor = clamp((float)(y1 - y), 0.0f, (float)(dimh.y)) / (float)(dimh.y);
    c2 *= factor;
    //c2 = clamp(c2, (uint4)(0,0,0,255), (uint4)(255,255,255,255));
    c2 = clamp(c2, (float4)(0,0,0,1), (float4)(1,1,1,1));
    c2 = (float4)(0, 0, 1.0f, 1.0f);
    write_imagef(dest, p, c2);
}

