/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Early animation support demo for Raydium

#include "raydium/index.c"

GLfloat sun[]={1.0,0.9,0.8,1.0};

GLfloat camx=3.01;
GLfloat camy=3;
GLfloat camz=0;


void display(void)
{
static float anm=0;
static int anmid=0;
 
raydium_joy_key_emul();
 
if(raydium_key_last==1027) exit(0);


raydium_clear_frame();


raydium_camera_look_at(0.4,-0.4,0, 0,-0.02,0); 


anm+=(raydium_frame_time*10);
if(raydium_key_last==100)
    {
    anmid--;
    anm=0;
    }

if(raydium_key_last==102)
    {
    anmid++;
    anm=0;
    }

raydium_object_anim(raydium_object_find("sas.tri"),0,anmid);
raydium_object_anim_frame_name("sas.tri",0,anm);

raydium_object_anim_instance_name("sas.tri",0);
raydium_object_draw_name("sas.tri");

raydium_osd_printf(7,20,15,0.5,"font2.tga","%i - Use left/right to switch animation",(int)anm);

raydium_rendering_finish();
}


int main(int argc, char **argv)
{
raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Animation test");
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=0.01;
raydium_projection_far=1000;
raydium_projection_fov=60;
raydium_window_view_update();

raydium_light_on(0);
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
raydium_light_intensity[0]=100000;

raydium_light_position[0][0]=50;
raydium_light_position[0][1]=150;
raydium_light_position[0][2]=200;

raydium_light_update_all(0);
raydium_background_color_change(sun[0],sun[1],sun[2],sun[3]);
raydium_fog_disable();

raydium_callback(&display);

return 0;
}
