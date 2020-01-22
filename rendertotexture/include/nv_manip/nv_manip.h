
#ifndef _nv_manip_h_
#define _nv_manip_h_

#include <nv_algebra.h>

class nv_manip
{
public:
	enum {
		LMOUSE_DN   = 0x00000001,
		MMOUSE_DN   = 0x00000002,
		RMOUSE_DN   = 0x00000004,
        MOUSE_DN    = 0x00000007, // mask for mouse keys down state
		CTRL_DN	    = 0x00000010,
		ALT_DN      = 0x00000020,
		SHIFT_DN    = 0x00000040,
        KEYBOARD_DN = 0x00000070, // mask for passive keys down state
		LMOUSE_UP   = 0x00000100,
		MMOUSE_UP   = 0x00000200,
		RMOUSE_UP   = 0x00000400,
        MOUSE_UP    = 0x00000700, // mask for mouse keys up state
	};

   	typedef enum {
        CAMERA      = 0x00000001,
        OBJECT      = 0x00000002,
        TRANSLATE   = 0x00000010,
        PAN         = 0x00000020,
        ROTATE      = 0x00000040,
        ALL         = 0x00000070,
    } nv_manip_behavior;

    nv_manip();

    void            set_manip_behavior(long flag);
    long            get_manip_behavior() const;

    virtual void    mouse_down(const vec2 & pt, int state);
	virtual void    mouse_up(const vec2 & pt, int state);
	virtual void    mouse_move(const vec2 & pt, int state);

    void            set_clip_planes(const nv_scalar & near_z, const nv_scalar & far_z );
    void            set_screen_size(const unsigned int & width, const unsigned int & height );

    // for OBJECT manipulation
    void            set_camera(const mat4 & camera);
    const mat4 &    get_camera() const;

    // for CAMERA manipulation
    void            set_eye_pos(const vec3 & eye_pos);
    const vec3 &    get_eye_pos() const;

    void            set_focus_pos(const vec3 & focus_pos);
    void            set_fov(const nv_scalar & fov);

	const mat4 &    get_mat() const;
    void            set_mat(const mat4 & m);

    unsigned int    get_screen_width();
    unsigned int    get_screen_height();
    double          get_screen_ratio();

    nv_scalar       get_fov();
    nv_scalar       get_near_z();
    nv_scalar       get_far_z();

    void            reset();

protected:
	typedef enum {
        _NIL,
        _TUMBLE,
        _DOLLY,
        _PAN
    } nv_manip_mode;

private:
    long            _behavior;
    nv_manip_mode   _mode;

	mat4	        _m,_p,_m_old,_camera;
    nv_scalar       _near_z;
    nv_scalar       _far_z;
    nv_scalar       _fov;
    unsigned int    _screen_width;
    unsigned int    _screen_height;
    double          _screen_ratio;
    vec2            _mouse_start;
    quat            _track_quat,_curquat;
    vec3            _eye_pos,_prev_eye_pos;
    vec3            _focus_pos,_prev_focus_pos;
    vec3            _eye_y;
};

#endif // _nv_manip_h_