

__kernel void filter(
	   global uchar4 * input,
	   global uchar4 * output,
	   uint width,
	   uint height)
{
	uint x = get_global_id(0);
	uint y = get_global_id(1);
	
	if (x >= width) return;
	if (y >= height) return;
	
	int2 p = (int2)(x, y);
	uint4 c = (uint4)(255,0,0,255);
//	write_imageui(output, p, c);
	output[y*width + x].r = 255;
	output[y*width + x].g = 255;
	output[y*width + x].b = 255;
	output[y*width + x].a = 255;
}