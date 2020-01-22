
#ifndef _nv_manip_h_
#include <nv_manip.h>
#endif // _nv_manip_h_

nv_manip::nv_manip() :
    _near_z(nv_zero),
    _far_z(nv_zero),
    _m(mat4_id),
    _p(mat4_id),
    _mouse_start(vec2_null),
    _eye_pos(vec3_null),
    _focus_pos(vec3_null),
    _mode(_NIL),
    _behavior(CAMERA|PAN|TRANSLATE|ROTATE),
    _track_quat(quat_id),
    _curquat(quat_id),
    _eye_y(vec3_y),
    _screen_ratio(nv_one)
{
}

void nv_manip::reset()
{
    _track_quat = quat_id;
    _curquat = quat_id;
    _mode = _NIL;
    _m = mat4_id;
    _mouse_start = vec2_null;
    _focus_pos = vec3_null;
}

void nv_manip::set_manip_behavior(long flag)
{
    _behavior = flag;
}

long nv_manip::get_manip_behavior() const
{
    return _behavior;
}

void nv_manip::mouse_down(const vec2 & pt, int state)
{
    if (state & LMOUSE_DN)
    {
        _m_old = _m;
        _mouse_start.x = pt.x;
        _mouse_start.y = pt.y;

        _track_quat = _curquat;

        _prev_eye_pos = _eye_pos;
        _prev_focus_pos = _focus_pos;

        if ( state & CTRL_DN)
            _mode = _DOLLY;
        else if ( state & SHIFT_DN)
            _mode = _PAN;
        else
            _mode = _TUMBLE;
    }
}

void nv_manip::mouse_up(const vec2 & pt, int state)
{
    if (state & LMOUSE_UP)
    {
        _mode = _NIL;
    }
}

void nv_manip::mouse_move(const vec2 & pt, int state)
{
    vec3 tmp;
    quat tmpquat;
    switch(_mode)
    {
        case _TUMBLE:
        {
            vec2 pt1( ( nv_two * _mouse_start.x - _screen_width) / _screen_width,
                      ( _screen_height - nv_two * _mouse_start.y ) / _screen_height);
            vec2 pt2( ( nv_two * pt.x - _screen_width) / _screen_width,
                      ( _screen_height - nv_two * pt.y  ) / _screen_height);

            trackball( tmpquat, pt1, pt2, nv_scalar(0.8));
            add_quats( _curquat, tmpquat, _track_quat);

            if (_behavior & CAMERA && _behavior & ROTATE)
            {
                mat3 M;
                sub(tmp,_prev_eye_pos,_focus_pos);
                quat_2_mat(M, _curquat );
                nv_scalar mag = nv_norm(tmp);

                _eye_y = vec3(M.a01, M.a11, M.a21);
                vec3 z(M.a02, M.a12, M.a22);

                scale(z,mag);
                add(_eye_pos,z,_focus_pos);

                look_at( _m, _eye_pos, _focus_pos, _eye_y);
            }
            else if (_behavior & OBJECT && _behavior & ROTATE)
            {
                _m.set_rot( _curquat );
            }
            break;
        }
        case _DOLLY:
        {
            nv_scalar z_delta = (pt.y - _mouse_start.y) / _screen_height * (_far_z - _near_z);
            
            if (_behavior & CAMERA && _behavior & TRANSLATE)
            {
                vec3 z;
                vec3 norm_z;
                sub(z,_prev_eye_pos,_prev_focus_pos);
                nv_scalar mag = nv_norm(z);
                norm_z = z;
                normalize(norm_z);

                vec3 z_offset(norm_z);
                scale(z_offset, z_delta);

                add(_eye_pos, _prev_eye_pos, z_offset);
                add(_focus_pos, _prev_focus_pos, z_offset);
                look_at( _m, _eye_pos, _focus_pos, _eye_y);
            }
            else if (_behavior & OBJECT && _behavior & TRANSLATE)
            {
                vec3 obj_pos = vec3(_m_old.x, _m_old.y, _m_old.z );
                sub( tmp, _eye_pos, obj_pos );
                scale( tmp, z_delta * nv_zero_5 );
                _m.x = obj_pos.x + tmp.x;
                _m.y = obj_pos.y + tmp.y;
                _m.z = obj_pos.z + tmp.z;
            }
            break;
        }
        case _PAN:
        {
            nv_scalar fov2 = -tan(nv_zero_5 * nv_to_rad * _fov ) * _m.z;
            nv_scalar winx = pt.x * nv_two / _screen_width - nv_one;
            nv_scalar winy = (_screen_height  - pt.y ) * nv_two / _screen_height  - nv_one;
            nv_scalar winz = _m_old.z;

            vec3 in(
                winx * fov2 * _screen_ratio,
                winy * fov2,
                winz
            );
            mat4 inv_m;
            vec3 offset;

            if (_behavior & CAMERA && _behavior & PAN)
            {
                invert(inv_m,_m);

                mult_dir(offset, inv_m, in);

                add(tmp, offset, _prev_eye_pos);
                scale(tmp,-nv_one);

                add(_eye_pos, _prev_eye_pos, tmp);
                add(_focus_pos, _prev_focus_pos, tmp);
                look_at( _m, _eye_pos, _focus_pos, _eye_y);
            }
            else if (_behavior & OBJECT && _behavior & PAN)
            {
                invert(inv_m,_camera);
                mult_dir(offset, inv_m, in);

                _m.x = offset.x;
                _m.y = offset.y;
                _m.z = offset.z;
            }
            break;
        }
        default:
            break;
    }
}

void nv_manip::set_clip_planes(const nv_scalar & near_z, const nv_scalar & far_z )
{
    _near_z = near_z;
    _far_z = far_z;
}

void nv_manip::set_fov(const nv_scalar & fov)
{
    _fov = fov;
}

void nv_manip::set_screen_size(const unsigned int & width, const unsigned int & height )
{
    _screen_width = width;
    _screen_height = height;
    _screen_ratio = (double)width/(double)height;
}

void nv_manip::set_eye_pos(const vec3 & eye_pos)
{
    _eye_pos = eye_pos;
    look_at( _m, _eye_pos, _focus_pos, vec3_y);
    _track_quat = quat_id;
    mat_2_quat(_curquat,_m);
    conj(_curquat);
}

const vec3 & nv_manip::get_eye_pos() const
{
    return _eye_pos;
}

void nv_manip::set_camera(const mat4 & camera)
{
    _camera = camera;
}

const mat4 & nv_manip::get_camera() const
{
    return _camera;
}

void nv_manip::set_focus_pos(const vec3 & focus_pos)
{
    _focus_pos = focus_pos;
    look_at( _m, _eye_pos, _focus_pos, vec3_y);
    mat_2_quat(_curquat,_m);
    conj(_curquat);

    _track_quat = quat_id;
}

const mat4 & nv_manip::get_mat() const
{
    return _m;
}

void nv_manip::set_mat(const mat4 & m)
{
    _m = m;
}

unsigned int nv_manip::get_screen_width()
{
    return _screen_width;
}

unsigned int nv_manip::get_screen_height()
{
    return _screen_height;
}

nv_scalar nv_manip::get_fov()
{
    return _fov;
}

nv_scalar nv_manip::get_near_z()
{
    return _near_z;
}

nv_scalar nv_manip::get_far_z()
{
    return _far_z;
}

double nv_manip::get_screen_ratio()
{
    return _screen_ratio;
}