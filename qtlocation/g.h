// -*- mode: c++ -*-

/***************************************************************************************************
 **
 ** $QTCARTO_BEGIN_LICENSE:GPL3$
 **
 ** Copyright (C) 2016 Fabrice Salvaire
 ** Contact: http://www.fabrice-salvaire.fr
 **
 ** This file is part of the Alpine Toolkit software.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **
 ** $QTCARTO_END_LICENSE$
 **
 ***************************************************************************************************/

/****************************************************************************
 **
 ** Copyright (C) 2015 The Qt Company Ltd.
 ** Contact: http://www.qt.io/licensing/
 **
 ** This file is part of the QtLocation module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL3$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see http://www.qt.io/terms-conditions. For further
 ** information use the contact form at http://www.qt.io/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPLv3 included in the
 ** packaging of this file. Please review the following information to
 ** ensure the GNU Lesser General Public License version 3 requirements
 ** will be met: https://www.gnu.org/licenses/lgpl.html.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 2.0 or later as published by the Free
 ** Software Foundation and appearing in the file LICENSE.GPL included in
 ** the packaging of this file. Please review the following information to
 ** ensure the GNU General Public License version 2.0 requirements will be
 ** met: http://www.gnu.org/licenses/gpl-2.0.html.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

/**************************************************************************************************/

#ifndef MAP_GESTURE_AREA_H
#define MAP_GESTURE_AREA_H

/**************************************************************************************************/

#include "coordinate/mercator.h"
#include "coordinate/wgs84.h"
#include "geo_coordinate_animation.h"
#include "geometry/vector.h"
#include "math/interval.h"

// #include <QtCore/QPointer>
// #include <QtGui/QVector2D>
// #include <QtPositioning/qgeocoordinate.h>

#include <QDebug> // Fixme: QtDebug ???
#include <QElapsedTimer>
#include <QTouchEvent>
#include <QtQuick/QQuickItem>

/**************************************************************************************************/

// QT_BEGIN_NAMESPACE

// class QGraphicsSceneMouseEvent;
// class QTouchEvent;
// class QWheelEvent;
// class QcGeoCoordinateAnimation;

class QcMapItem;

/**************************************************************************************************/

class QcMapPinchEvent : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QcVectorDouble center READ center)
  Q_PROPERTY(qreal angle READ angle)
  Q_PROPERTY(QcVectorDouble point1 READ point1)
  Q_PROPERTY(QcVectorDouble point2 READ point2)
  Q_PROPERTY(int number_of_points READ number_of_points)
  Q_PROPERTY(bool accepted READ accepted WRITE set_accepted)

public:
  QcMapPinchEvent(const QcVectorDouble & center,
                  qreal angle,
                  const QcVectorDouble & point1,
                  const QcVectorDouble & point2,
                  int number_of_points = 0,
                  bool accepted = true)
    : QObject()
    , m_center(center)
    , m_point1(point1)
    , m_point2(point2)
    , m_angle(angle)
    , m_number_of_points(number_of_points)
    , m_accepted(accepted)
  {}
  QcMapPinchEvent()
    : QObject()
    , m_angle(0.0)
    , m_number_of_points(0)
    , m_accepted(true)
  {}

  QcVectorDouble center() const
  {
    return m_center;
  }
  void set_center(const QcVectorDouble & center)
  {
    m_center = center;
  }

  qreal angle() const
  {
    return m_angle;
  }
  void set_angle(qreal angle)
  {
    m_angle = angle;
  }

  QcVectorDouble point1() const
  {
    return m_point1;
  }
  void set_point1(const QcVectorDouble & p)
  {
    m_point1 = p;
  }

  QcVectorDouble point2() const
  {
    return m_point2;
  }
  void set_point2(const QcVectorDouble & p)
  {
    m_point2 = p;
  }

  int number_of_points() const
  {
    return m_number_of_points;
  }
  void set_number_of_points(int number_of_points)
  {
    m_number_of_points = number_of_points;
  }

  bool accepted() const
  {
    return m_accepted;
  }
  void set_accepted(bool status)
  {
    m_accepted = status;
  }

private:
  QcVectorDouble m_center;
  QcVectorDouble m_point1;
  QcVectorDouble m_point2;
  qreal m_angle;
  int m_number_of_points;
  bool m_accepted;
};

/**************************************************************************************************/

class QcMapGestureArea : public QQuickItem
{
  Q_OBJECT
  Q_ENUMS(GeoMapGesture)
  Q_FLAGS(AcceptedGestures)

  Q_PROPERTY(bool enabled READ enabled WRITE set_enabled NOTIFY enabledChanged)
  Q_PROPERTY(bool pinch_active READ is_pinch_active NOTIFY pinch_activeChanged)
  Q_PROPERTY(bool pan_active READ is_pan_active NOTIFY pan_activeChanged)
  Q_PROPERTY(bool rotation_active READ is_rotation_active NOTIFY rotation_activeChanged)
  Q_PROPERTY(bool tilt_active READ is_tilt_active NOTIFY tilt_activeChanged)
  Q_PROPERTY(AcceptedGestures accepted_gestures READ accepted_gestures WRITE set_accepted_gestures NOTIFY accepted_gesturesChanged)
  Q_PROPERTY(qreal maximum_zoom_level_change READ maximum_zoom_level_change WRITE set_maximum_zoom_level_change NOTIFY maximum_zoom_level_changeChanged)
  Q_PROPERTY(qreal flick_deceleration READ flick_deceleration WRITE set_flick_deceleration NOTIFY flick_decelerationChanged)
  Q_PROPERTY(bool prevent_stealing READ prevent_stealing WRITE set_prevent_stealing NOTIFY prevent_stealingChanged)

public:
  QcMapGestureArea(QcMapItem * map);
  ~QcMapGestureArea();

  enum GeoMapGesture {
    NoGesture = 0x0000,
    PinchGesture = 0x0001,
    PanGesture = 0x0002,
    FlickGesture = 0x0004,
    RotationGesture = 0x0008,
    TiltGesture = 0x0010
  };

  Q_DECLARE_FLAGS(AcceptedGestures, GeoMapGesture)

  AcceptedGestures accepted_gestures() const;
  void set_accepted_gestures(AcceptedGestures accepted_gestures);

  bool is_pinch_active() const;
  bool is_rotation_active() const;
  bool is_tilt_active() const;
  bool is_pan_active() const;
  bool is_active() const;

  bool enabled() const;
  void set_enabled(bool enabled);

  qreal maximum_zoom_level_change() const;
  void set_maximum_zoom_level_change(qreal max_change);

  qreal flick_deceleration() const;
  void set_flick_deceleration(qreal deceleration);

  // void set_zoom_level_interval(const QcIntervalInt interval);

  // bool prevent_stealing() const { return m_prevent_stealing; }
  // void set_prevent_stealing(bool prevent);

  void handle_touch_event(QTouchEvent * event);
  // #if QT_CONFIG(wheelevent)
  void handle_wheel_event(QWheelEvent * event);
  // #endif
  void handle_mouse_press_event(QMouseEvent * event);
  void handle_mouse_move_event(QMouseEvent * event);
  void handle_mouse_release_event(QMouseEvent * event);
  void handle_mouse_ungrab_event();
  void handle_touch_ungrab_event();

  // void set_minimum_zoom_level(qreal min);
  // qreal minimum_zoom_level() const;

  // void set_maximum_zoom_level(qreal max);
  // qreal maximum_zoom_level() const;

  void set_map(QcMapItem * map);

  bool prevent_stealing() const;
  void set_prevent_stealing(bool prevent);

Q_SIGNALS:
  void pan_activeChanged();
  void pinch_activeChanged();
  void rotation_activeChanged();
  void tilt_activeChanged();
  void enabledChanged();
  void maximum_zoom_level_changeChanged();
  void accepted_gesturesChanged();
  void flick_decelerationChanged();
  void pinch_started(QcMapPinchEvent * pinch);
  void pinch_updated(QcMapPinchEvent * pinch);
  void pinch_finished(QcMapPinchEvent * pinch);
  void pan_started();
  void pan_finished();
  void flick_started();
  void flick_finished();
  void rotation_started(QcMapPinchEvent * pinch);
  void rotation_updated(QcMapPinchEvent * pinch);
  void rotation_finished(QcMapPinchEvent * pinch);
  void tilt_started(QcMapPinchEvent * pinch);
  void tilt_updated(QcMapPinchEvent * pinch);
  void tilt_finished(QcMapPinchEvent * pinch);
  void prevent_stealingChanged();

private:
  void update();

  // Create general data relating to the touch points
  void touch_point_state_machine();
  void start_one_touch_point();
  void update_one_touch_point();
  void start_two_touch_points();
  void update_two_touch_points();

  // All two fingers vertical parallel panning related code, which encompasses tilting
  void tilt_state_machine();
  bool can_start_tilt();
  void start_tilt();
  void update_tilt();
  void end_tilt();

  // All two fingers rotation related code, which encompasses rotation
  void rotation_state_machine();
  bool can_start_rotation();
  void start_rotation();
  void update_rotation();
  void end_rotation();

  // All pinch related code, which encompasses zoom
  void pinch_state_machine();
  bool can_start_pinch();
  void start_pinch();
  void update_pinch();
  void end_pinch();

  // Pan related code (regardles of number of touch points),
  // includes the flick based panning after letting go
  void pan_state_machine();
  bool can_start_pan();
  void update_pan();
  bool try_start_flick();
  void start_flick(int dx, int dy, int time_ms = 0);
  void stop_flick();

  bool pinch_enabled() const;
  void set_pinch_enabled(bool enabled);
  bool rotation_enabled() const;
  void set_rotation_enabled(bool enabled);
  bool tilt_enabled() const;
  void set_tilt_enabled(bool enabled);
  bool pan_enabled() const;
  void set_pan_enabled(bool enabled);
  bool flick_enabled() const;
  void set_flick_enabled(bool enabled);

  // private slots:
private Q_SLOTS:
  void handle_flick_animation_stopped();

private:
  void stop_pan();
  void clear_touch_data();
  void update_flick_parameters(const QcVectorDouble & pos);

private:
  QcMapItem * m_map;
  QcMapItem * m_declarative_map;
  bool m_enabled;

  // This should be intended as a "two fingers gesture" struct
  struct Pinch
  {
    Pinch()
      : m_pinch_enabled(true)
      , m_rotation_enabled(true)
      , m_tilt_enabled(true)
      , m_start_distanceanceanceance(0)
      , m_last_angle(0.0)
    {}

    QcMapPinchEvent m_event;
    bool m_pinch_enabled;
    bool m_rotation_enabled;
    bool m_tilt_enabled;
    struct Zoom
    {
      Zoom()
        : m_interval()
        ,
        // m_minimum(0.0),
        // m_maximum(30.0),
        m_start(0.0)
        , m_previous(0.0)
        , maximum_change(4.0)
      {}
      QcIntervalInt m_interval;
      // qreal m_minimum;
      // qreal m_maximum;
      qreal m_start;
      qreal m_previous;
      qreal maximum_change;
    } m_zoom;

    struct Rotation
    {
      Rotation()
        : m_start_bearing(0.0)
        , m_previous_touch_angle(0.0)
        , m_total_angle(0.0)
      {}
      qreal m_start_bearing;
      qreal m_previous_touch_angle; // needed for detecting crossing +- 180 in a safer way
      qreal m_total_angle;
    } m_rotation;

    struct Tilt
    {
      Tilt() {}
      QcVectorDouble m_start_touch_centroid;
      qreal m_start_tilt;
    } m_tilt;

    QcVectorDouble m_last_point1;
    QcVectorDouble m_last_point2;
    qreal m_start_distanceanceanceance;
    qreal m_last_angle;
  } m_pinch;

  AcceptedGestures m_accepted_gestures;

  struct Pan
  {
    Pan()
      : m_flick_enabled(true)
      , m_pan_enabled(true)
      , m_max_velocity(2500)
      , m_deceleration(2500)
      , m_animation(0)
    {}
    bool m_flick_enabled;
    bool m_pan_enabled;
    qreal m_max_velocity;
    qreal m_deceleration;
    QcGeoCoordinateAnimation * m_animation;
  } m_flick;

  // these are calculated regardless of gesture or number of touch points
  QVector2D m_flick_vector;
  QElapsedTimer m_last_positionitionitionitionition_time;
  QcVectorDouble m_last_positionitionitionition;
  QList<QTouchEvent::TouchPoint> m_all_points;
  QList<QTouchEvent::TouchPoint> m_touch_points;
  QScopedPointer<QTouchEvent::TouchPoint> m_mouse_point;
  QcVectorDouble m_scene_start_point1;

  // only set when two points in contact
  QcVectorDouble m_scene_start_point2;
  QGeoCoordinate m_start_coordinateinateinateinate;
  QGeoCoordinate m_touch_center_coordinateinateinateinate;
  qreal m_two_touch_angle;
  qreal m_two_touch_angle_start;
  qreal m_distance_between_touch_points;
  qreal m_distance_between_touch_points_start;
  QcVectorDouble m_two_touch_points_centroid_start;
  QcVectorDouble m_touch_points_centroid;
  bool m_prevent_stealing;
  bool m_pan_enabled;

private:
  // prototype state machine...

  enum TouchPointState {
    TouchPoints0,
    TouchPoints1,
    TouchPoints2
  } m_touch_point_state;

  enum PinchState {
    PinchInactive,
    PinchInactiveTwoPoints,
    PinchActive
  } m_pinch_state;

  enum RotationState {
    RotationInactive,
    RotationInactiveTwoPoints,
    RotationActive
  } m_rotation_state;

  enum TiltState {
    TiltInactive,
    TiltInactiveTwoPoints,
    TiltActive
  } m_tilt_state;

  enum FlickState {
    FlickInactive,
    PanActive,
    FlickActive
  } m_flick_state;

  inline void set_touch_point_state(const TouchPointState state);
  inline void set_flick_state(const FlickState state);
  inline void set_tilt_state(const TiltState state);
  inline void set_rotation_state(const RotationState state);
  inline void set_pinch_state(const PinchState state);
};

// QT_END_NAMESPACE

QML_DECLARE_TYPE(QcMapGestureArea)

#endif // MAP_GESTURE_AREA_H
