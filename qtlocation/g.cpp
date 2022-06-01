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

// #include "map_gesture_area.h"
#include "g.h"
#include "qtcarto.h"

#include "declarative_map_item.h"

#include <cmath>

#include <QDebug>
#include <QPropertyAnimation>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>
// #if QT_CONFIG(wheelevent)
#include <QtGui/QWheelEvent>
// #endif
#include <QtQuick/QQuickWindow>

/**************************************************************************************************/

// #include <QtPositioningQuick/private/qquickgeocoordinateanimation_p.h>
// #include "qdeclarativegeomap_p.h"
// #include "error_messages_p.h"

// #include <QtGui/qevent.h>
// #include <QtQml/qqmlinfo.h>
// #include "math.h"
// #include "qgeomap_p.h"
// #include "qdoublevector2d_p.h"
// #include "qlocationutils_p.h"
// #include <QtGui/QMatrix4x4>

/**************************************************************************************************/

/*!
  \qmltype MapPinchEvent
  \instantiates QcMapPinchEvent
  \inqmlmodule QtLocation

  \brief MapPinchEvent type provides basic information about pinch event.

  MapPinchEvent type provides basic information about pinch event. They are
  present in handlers of MapPinch (for example pinch_started/pinch_updated). Events are only
  guaranteed to be valid for the duration of the handler.

  Except for the \l accepted property, all properties are read-only.

  \section2 Example Usage

  The following example enables the pinch gesture on a map and reacts to the
  finished event.

  \code
  Map {
  id: map
  gesture.enabled: true
  gesture.onPinchFinished:{
  var coordinate1 = map.toCoordinate(gesture.point1)
  var coordinate2 = map.toCoordinate(gesture.point2)
  console.log("Pinch started at:")
  console.log("        Points (" + gesture.point1.x + ", " + gesture.point1.y + ") - (" + gesture.point2.x + ", " + gesture.point2.y + ")")
  console.log("   Coordinates (" + coordinate1.latitude + ", " + coordinate1.longitude + ") - (" + coordinate2.latitude + ", " + coordinate2.longitude + ")")
  }
  }
  \endcode

  \ingroup qml-QtLocation5-maps
*/

/*!
  \qmlproperty QPoint QtLocation::MapPinchEvent::center

  This read-only property holds the current center point.
*/

/*!
  \qmlproperty real QtLocation::MapPinchEvent::angle

  This read-only property holds the current angle between the two points in
  the range -180 to 180. Positive values for the angles mean counter-clockwise
  while negative values mean the clockwise direction. Zero degrees is at the
  3 o'clock position.
*/

/*!
  \qmlproperty QPoint QtLocation::MapPinchEvent::point1
  \qmlproperty QPoint QtLocation::MapPinchEvent::point2

  These read-only properties hold the actual touch points generating the pinch.
  The points are not in any particular order.
*/

/*!
  \qmlproperty int QtLocation::MapPinchEvent::number_of_points

  This read-only property holds the number of points currently touched.
  The MapPinch will not react until two touch points have initiated a gesture,
  but will remain active until all touch points have been released.
*/

/*!
  \qmlproperty bool QtLocation::MapPinchEvent::accepted

  Setting this property to false in the \c MapPinch::onPinchStarted handler
  will result in no further pinch events being generated, and the gesture
  ignored.
*/

/*!
  \qmltype MapGestureArea
  \instantiates QcMapGestureArea

  \inqmlmodule QtLocation

  \brief The MapGestureArea type provides Map gesture interaction.

  MapGestureArea objects are used as part of a Map, to provide for panning,
  flicking and pinch-to-zoom gesture used on touch displays, as well as two finger rotation
  and two finger parallel vertical sliding to tilt the map.
  On platforms supporting \l QWheelEvent, using the scroll wheel alone, or in combination with
  key modifiers Shift or Control will also zoom, rotate or tilt the map, respectively.

  A MapGestureArea is automatically created with a new Map and available with
  the \l{Map::gesture}{gesture} property. This is the only way
  to create a MapGestureArea, and once created this way cannot be destroyed
  without its parent Map.

  The two most commonly used properties of the MapGestureArea are the \l enabled
  and \l accepted_gestures properties. Both of these must be set before a
  MapGestureArea will have any effect upon interaction with the Map.
  The \l flick_deceleration property controls how quickly the map pan slows after contact
  is released while panning the map.

  \section2 Performance

  The MapGestureArea, when enabled, must process all incoming touch events in
  order to track the shape and size of the "pinch". The overhead added on
  touch events can be considered constant time.

  \section2 Example Usage

  The following example enables the pinch and pan gestures on the map, but not flicking. So the
  map scrolling will halt immediately on releasing the mouse button / touch.

  \code
  Map {
  gesture.enabled: true
  gesture.accepted_gestures: MapGestureArea.PinchGesture | MapGestureArea.PanGesture
  }
  \endcode

  \ingroup qml-QtLocation5-maps
*/

/*!
  \qmlproperty bool QtLocation::MapGestureArea::enabled

  This property holds whether the gestures are enabled.
*/

/*!
  \qmlproperty bool QtLocation::MapGestureArea::pinch_active

  This read-only property holds whether the pinch gesture is active.
*/

/*!
  \qmlproperty bool QtLocation::MapGestureArea::pan_active

  This read-only property holds whether the pan gesture is active.

  \note Change notifications for this property were introduced in Qt 5.5.
*/

/*!
  \qmlproperty bool QtLocation::MapGestureArea::rotation_active

  This read-only property holds whether the two-finger rotation gesture is active.
*/

/*!
  \qmlproperty bool QtLocation::MapGestureArea::tilt_active

  This read-only property holds whether the two-finger tilt gesture is active.
*/

/*!
  \qmlproperty real QtLocation::MapGestureArea::maximum_zoom_level_change

  This property holds the maximum zoom level change per pinch, essentially
  meant to be used for setting the zoom sensitivity.

  It is an indicative measure calculated from the dimensions of the
  map area, roughly corresponding how much zoom level could change with
  maximum pinch zoom. Default value is 4.0, maximum value is 10.0
*/

/*!
  \qmlproperty real MapGestureArea::flick_deceleration

  This property holds the rate at which a flick will decelerate.

  The default value is 2500.
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::pinch_started(PinchEvent event)

  This signal is emitted when a pinch gesture is started.

  Information about the pinch event is provided in \a event.

  The corresponding handler is \c onPinchStarted.

  \sa pinch_updated, pinch_finished
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::pinch_updated(PinchEvent event)

  This signal is emitted as the user's fingers move across the map,
  after the \l pinch_started signal is emitted.

  Information about the pinch event is provided in \a event.

  The corresponding handler is \c onPinchUpdated.

  \sa pinch_started, pinch_finished
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::pinch_finished(PinchEvent event)

  This signal is emitted at the end of a pinch gesture.

  Information about the pinch event is provided in \a event.

  The corresponding handler is \c onPinchFinished.

  \sa pinch_started, pinch_updated
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::pan_started()

  This signal is emitted when the map begins to move due to user
  interaction. Typically this means that the user is dragging a finger -
  or a mouse with one of more mouse buttons pressed - on the map.

  The corresponding handler is \c onPanStarted.
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::pan_finished()

  This signal is emitted when the map stops moving due to user
  interaction.  If a flick was generated, this signal is
  emitted before flick starts. If a flick was not
  generated, this signal is emitted when the
  user stops dragging - that is a mouse or touch release.

  The corresponding handler is \c onPanFinished.

*/

/*!
  \qmlsignal QtLocation::MapGestureArea::flick_started()

  This signal is emitted when the map is flicked.  A flick
  starts from the point where the mouse or touch was released,
  while still in motion.

  The corresponding handler is \c onFlickStarted.
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::flick_finished()

  This signal is emitted when the map stops moving due to a flick.

  The corresponding handler is \c onFlickFinished.
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::rotation_started(PinchEvent event)

  This signal is emitted when a two-finger rotation gesture is started.

  Information about the pinch event is provided in \a event.

  The corresponding handler is \c onRotationStarted.

  \sa rotation_updated(), rotation_finished()
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::rotation_updated(PinchEvent event)

  This signal is emitted as the user's fingers move across the map,
  after the \l rotation_started() signal is emitted.

  Information about the pinch event is provided in \a event.

  The corresponding handler is \c onRotationUpdated.

  \sa rotation_started(), rotation_finished()
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::rotation_finished(PinchEvent event)

  This signal is emitted at the end of a two-finger rotation gesture.

  Information about the pinch event is provided in \a event.

  The corresponding handler is \c onRotationFinished.

  \sa rotation_started(), rotation_updated()
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::tilt_started(PinchEvent event)

  This signal is emitted when a two-finger tilt gesture is started.

  Information about the pinch event is provided in \a event.

  The corresponding handler is \c onTiltStarted.

  \sa tilt_updated(), tilt_finished()
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::tilt_updated(PinchEvent event)

  This signal is emitted as the user's fingers move across the map,
  after the \l tilt_started signal is emitted.

  Information about the pinch event is provided in \a event.

  The corresponding handler is \c onTiltUpdated.

  \sa tilt_started(), tilt_finished()
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::tilt_finished(PinchEvent event)

  This signal is emitted at the end of a two-finger tilt gesture.

  Information about the pinch event is provided in \a event.

  The corresponding handler is \c onTiltFinished.

  \sa tilt_started(), tilt_updated()
*/

/**************************************************************************************************/

QT_BEGIN_NAMESPACE

// [px/s]
// constexpr int QML_MAP_FLICK_DEFAULT_MAX_VELOCITY = 2500;
constexpr int QML_MAP_FLICK_MINIMUM_DECELERATION = 500;
// constexpr int QML_MAP_FLICK_DEFAULT_DECELERATION = 2500;
constexpr int QML_MAP_FLICK_MAXIMUM_DECELERATION = 10000;
constexpr int QML_MAP_FLICK_VELOCITY_SAMPLE_PERIOD = 38;

// FlickThreshold determines how far the "mouse" must have moved
// before we perform a flick.
static const int FlickThreshold = 20;
// Really slow flicks can be annoying.
static const qreal MinimumFlickVelocity = 75.0;
// Tolerance for detecting two finger sliding start
static const qreal MaximumParallelPosition = 40.0; // in degrees
// Tolerance for detecting parallel sliding
static const qreal MaximumParallelSlidingAngle = 4.0; // in degrees
// Tolerance for starting rotation
static const qreal MinimumRotationStartingAngle = 15.0; // in degrees
// Tolerance for starting pinch
static const qreal MinimumPinchDelta = 40; // in pixels
// Tolerance for starting tilt when sliding vertical
static const qreal MinimumPanToTiltDelta = 80; // in pixels;

/**************************************************************************************************/

static qreal
distance_between_touch_points(const QcVectorDouble &p1, const QcVectorDouble &p2)
{
  return QLineF(p1, p2).length();
}

static qreal
angle_from_points(const QcVectorDouble &p1, const QcVectorDouble &p2)
{
  return QLineF(p1, p2).angle();
}

// Keeps it in +- 180
static qreal
touch_angle(const QcVectorDouble &p1, const QcVectorDouble &p2)
{
  auto angle = angle_from_points(p1, p2);
  if (angle > 180)
    angle -= 360;
  return angle;
}

// Deals with angles crossing the +-180 edge, assumes that the delta can't be > 180
static qreal
angle_delta(const qreal angle1, const qreal angle2)
{
  auto delta = angle1 - angle2;
  if (delta > 180.0) // detect crossing angle1 positive, angle2 negative, rotation counterclockwise, difference negative
    delta = angle1 - angle2 - 360.0;
  else if (delta < -180.0) // detect crossing angle1 negative, angle2 positive, rotation clockwise, difference positive
    delta = angle1 - angle2 + 360.0;

  return delta;
}

static bool
point_dragged(const QcVectorDouble &p_old, const QcVectorDouble &p_new)
{
  static const int start_drag_distance = qApp->styleHints()->startDragDistance();
  return ( qAbs(p_new.x() - p_old.x()) > start_drag_distance
           || qAbs(p_new.y() - p_old.y()) > start_drag_distance );
}

static qreal
vector_size(const QcVectorDouble &vector)
{
  return std::sqrt(vector.x() * vector.x() + vector.y() * vector.y());
}

// This linearizes the angles around 0, and keep it linear around 180, allowing to differentiate
// touch angles that are supposed to be parallel (0 or 180 depending on what finger goes first)
static qreal
touch_angle_tilting(const QcVectorDouble &p1, const QcVectorDouble &p2)
{
  qreal angle = angle_from_points(p1, p2);
  if (angle > 270)
    angle -= 360;
  return angle;
}

static bool
moving_parallel_vertical(const QcVectorDouble &p1_old, const QcVectorDouble &p1_new, const QcVectorDouble &p2_old, const QcVectorDouble &p2_new)
{
  if (!point_dragged(p1_old, p1_new) || !point_dragged(p2_old, p2_new))
    return false;

  QcVectorDouble v1 = p1_new - p1_old;
  QcVectorDouble v2 = p2_new - p2_old;
  qreal v1v2size = vector_size(v1 + v2);

  if (v1v2size < vector_size(v1) || v1v2size < vector_size(v2)) // going in opposite directions
    return false;

  const qreal new_angle = touch_angle_tilting(p1_new, p2_new);
  const qreal old_angle = touch_angle_tilting(p1_old, p2_old);
  const qreal angle_diff = angle_delta(new_angle, old_angle);

  if (qAbs(angle_diff) > MaximumParallelSlidingAngle)
    return false;

  return true;
}

/**************************************************************************************************/

QcMapGestureArea::QcMapGestureArea(QcMapItem *map)
  : QQuickItem(map),
    m_map(0),
    m_declarative_map(map),
    m_enabled(true),
    m_accepted_gestures(PinchGesture | PanGesture | FlickGesture | RotationGesture | TiltGesture),
    m_prevent_stealing(false)
{
  m_touch_point_state = TouchPoints0;
  m_pinch_state = PinchInactive;
  m_flick_state = FlickInactive;
  m_rotation_state = RotationInactive;
  m_tilt_state = TiltInactive;
}

/// \internal
void
QcMapGestureArea::set_map(QcMapItem *map)
{
  if (m_map || !map)
    return;

  m_map = map;
  m_flick.m_animation = new QcGeoCoordinateAnimation(this);
  m_flick.m_animation->setTargetObject(m_declarative_map);
  m_flick.m_animation->setProperty(QStringLiteral("center"));
  m_flick.m_animation->setEasing(QEasingCurve(QEasingCurve::OutQuad));
  connect(m_flick.m_animation, &QQuickAbstractAnimation::stopped, this, &QcMapGestureArea::handle_flick_animation_stopped);
  m_map->set_accepted_gestures(pan_enabled(), flick_enabled(), pinch_enabled(), rotation_enabled(), tilt_enabled());
}

/*!
  \qmlproperty bool QtQuick::MapGestureArea::prevent_stealing
  This property holds whether the mouse events may be stolen from this
  MapGestureArea.

  If a Map is placed within an item that filters child mouse
  and touch events, such as Flickable, the mouse and touch events
  may be stolen from the MapGestureArea if a gesture is recognized
  by the parent item, e.g. a flick gesture.  If prevent_stealing is
  set to \c true, no item will steal the mouse and touch events.

  Note that setting prevent_stealing to \c true once an item has started
  stealing events has no effect until the next press event.

  By default this property is set to \c false.
*/

bool
QcMapGestureArea::prevent_stealing() const
{
  return m_prevent_stealing;
}

void
QcMapGestureArea::set_prevent_stealing(bool prevent)
{
  if (prevent != m_prevent_stealing) {
    m_prevent_stealing = prevent;
    m_declarative_map->setKeepMouseGrab(m_prevent_stealing && m_enabled);
    m_declarative_map->setKeepTouchGrab(m_prevent_stealing && m_enabled);
    emit prevent_stealingChanged();
  }
}

QcMapGestureArea::~QcMapGestureArea()
{
}

/*!
  \qmlproperty enumeration QtLocation::MapGestureArea::accepted_gestures

  This property holds a bit field of gestures that are accepted. By default,
  all gestures are enabled.

  \value MapGestureArea.NoGesture
  Don't support any additional gestures (value: 0x0000).

  \value MapGestureArea.PinchGesture
  Support the map pinch gesture (value: 0x0001).

  \value MapGestureArea.PanGesture
  Support the map pan gesture (value: 0x0002).

  \value MapGestureArea.FlickGesture
  Support the map flick gesture (value: 0x0004).

  \value MapGestureArea.RotationGesture
  Support the map rotation gesture (value: 0x0008).

  \value MapGestureArea.TiltGesture
  Support the map tilt gesture (value: 0x0010).
*/

QcMapGestureArea::AcceptedGestures
QcMapGestureArea::accepted_gestures() const
{
  return m_accepted_gestures;
}

void
QcMapGestureArea::set_accepted_gestures(AcceptedGestures accepted_gestures)
{
  if (accepted_gestures == m_accepted_gestures)
    return;
  m_accepted_gestures = accepted_gestures;

  if (enabled()) {
    set_pan_enabled(accepted_gestures & PanGesture);
    set_flick_enabled(accepted_gestures & FlickGesture);
    set_pinch_enabled(accepted_gestures & PinchGesture);
    set_rotation_enabled(accepted_gestures & RotationGesture);
    set_tilt_enabled(accepted_gestures & TiltGesture);
  }

  if (m_map)
    m_map->set_accepted_gestures(pan_enabled(), flick_enabled(), pinch_enabled(), rotation_enabled(), tilt_enabled());

  emit accepted_gesturesChanged();
}

/// \internal
bool
QcMapGestureArea::is_pinch_active() const
{
  return m_pinch_state == PinchActive;
}

/// \internal
bool
QcMapGestureArea::is_rotation_active() const
{
  return m_rotation_state == RotationActive;
}

/// \internal
bool
QcMapGestureArea::is_tilt_active() const
{
  return m_tilt_state == TiltActive;
}

/// \internal
bool
QcMapGestureArea::is_pan_active() const
{
  return m_flick_state == PanActive || m_flick_state == FlickActive;
}

/// \internal
bool
QcMapGestureArea::enabled() const
{
  return m_enabled;
}

/// \internal
void
QcMapGestureArea::set_enabled(bool enabled)
{
  if (enabled == m_enabled)
    return;
  m_enabled = enabled;

  if (enabled) {
    set_pan_enabled(m_accepted_gestures & PanGesture);
    set_flick_enabled(m_accepted_gestures & FlickGesture);
    set_pinch_enabled(m_accepted_gestures & PinchGesture);
    set_rotation_enabled(m_accepted_gestures & RotationGesture);
    set_tilt_enabled(m_accepted_gestures & TiltGesture);
  } else {
    set_pan_enabled(false);
    set_flick_enabled(false);
    set_pinch_enabled(false);
    set_rotation_enabled(false);
    set_tilt_enabled(false);
  }
  if (m_map)
    m_map->set_accepted_gestures(pan_enabled(), flick_enabled(), pinch_enabled(), rotation_enabled(), tilt_enabled());

  emit enabledChanged();
}

/// \internal
bool
QcMapGestureArea::pinch_enabled() const
{
  return m_pinch.m_pinch_enabled;
}

/// \internal
void
QcMapGestureArea::set_pinch_enabled(bool enabled)
{
  m_pinch.m_pinch_enabled = enabled;
}

/// \internal
bool
QcMapGestureArea::rotation_enabled() const
{
  return m_pinch.m_rotation_enabled;
}

/// \internal
void
QcMapGestureArea::set_rotation_enabled(bool enabled)
{
  m_pinch.m_rotation_enabled = enabled;
}

/// \internal
bool
QcMapGestureArea::tilt_enabled() const
{
  return m_pinch.m_tilt_enabled;
}

/// \internal
void
QcMapGestureArea::set_tilt_enabled(bool enabled)
{
  m_pinch.m_tilt_enabled = enabled;
}

/// \internal
bool
QcMapGestureArea::pan_enabled() const
{
  return m_flick.m_pan_enabled;
}

/// \internal
void
QcMapGestureArea::set_pan_enabled(bool enabled)
{
  if (enabled == m_flick.m_pan_enabled)
    return;
  m_flick.m_pan_enabled = enabled;

  // unlike the pinch, the pan existing functionality is to stop immediately
  if (!enabled) {
    stop_pan();
    m_flick_state = FlickInactive;
  }
}

/// \internal
bool
QcMapGestureArea::flick_enabled() const
{
  return m_flick.m_flick_enabled;
}

/// \internal
void
QcMapGestureArea::set_flick_enabled(bool enabled)
{
  if (enabled == m_flick.m_flick_enabled)
    return;
  m_flick.m_flick_enabled = enabled;
  // unlike the pinch, the flick existing functionality is to stop immediately
  if (!enabled) {
    bool stateActive = (m_flick_state != FlickInactive);
    stop_flick();
    if (stateActive) {
      if (m_flick.m_pan_enabled)
        m_flick_state = PanActive;
      else
        m_flick_state = FlickInactive;
    }
  }
}

/// \internal
/// Used internally to set the minimum zoom level of the gesture area.
/// The caller is responsible to only send values that are valid
/// for the map plugin. Negative values are ignored.
void
QcMapGestureArea::set_minimum_zoom_level(qreal min)
{
  // TODO: remove m_zoom.m_minimum and m_maximum and use m_declarative_map directly instead.
  if (min >= 0)
    m_pinch.m_zoom.m_minimum = min;
}

/// \internal
qreal
QcMapGestureArea::minimum_zoom_level() const
{
  return m_pinch.m_zoom.m_minimum;
}

/// \internal
/// Used internally to set the maximum zoom level of the gesture area.
/// The caller is responsible to only send values that are valid
/// for the map plugin. Negative values are ignored.
void
QcMapGestureArea::set_maximum_zoom_level(qreal max)
{
  if (max >= 0)
    m_pinch.m_zoom.m_maximum = max;
}

/// \internal
qreal
QcMapGestureArea::maximum_zoom_level() const
{
  return m_pinch.m_zoom.m_maximum;
}

/// \internal
qreal
QcMapGestureArea::maximum_zoom_level_change() const
{
  return m_pinch.m_zoom.maximum_change;
}

/// \internal
void
QcMapGestureArea::set_maximum_zoom_level_change(qreal max_change)
{
  if (max_change == m_pinch.m_zoom.maximum_change || max_change < 0.1 || max_change > 10.0)
    return;
  m_pinch.m_zoom.maximum_change = max_change;
  emit maximum_zoom_level_changeChanged();
}

/// \internal
qreal
QcMapGestureArea::flick_deceleration() const
{
  return m_flick.m_deceleration;
}

/// \internal
void
QcMapGestureArea::set_flick_deceleration(qreal deceleration)
{
  if (deceleration < QML_MAP_FLICK_MINIMUM_DECELERATION)
    deceleration = QML_MAP_FLICK_MINIMUM_DECELERATION;
  else if (deceleration > QML_MAP_FLICK_MAXIMUM_DECELERATION)
    deceleration = QML_MAP_FLICK_MAXIMUM_DECELERATION;
  if (deceleration == m_flick.m_deceleration)
    return;
  m_flick.m_deceleration = deceleration;
  emit flick_decelerationChanged();
}

/// \internal
QTouchEvent::TouchPoint* createTouchPointFromMouseEvent(QMouseEvent *event, Qt::TouchPointState state)
{
  // this is only partially filled. But since it is only partially used it works
  // more robust would be to store a list of QcVectorDoubles rather than TouchPoints
  QTouchEvent::TouchPoint* newPoint = new QTouchEvent::TouchPoint();
  newPoint->setPos(event->position());
  newPoint->setScenePos(event->windowPos());
  newPoint->setScreenPos(event->screenPos());
  newPoint->setState(state);
  newPoint->setId(0);
  return newPoint;
}

/// \internal
void QcMapGestureArea::handle_mouse_press_event(QMouseEvent *event)
{
  if (m_map && m_map->handleEvent(event)) {
    event->accept();
    return;
  }

  m_mouse_point.reset(createTouchPointFromMouseEvent(event, QEventPoint::State::Pressed));
  if (m_touch_points.isEmpty())
    update();
  event->accept();
}

/// \internal
void QcMapGestureArea::handle_mouse_move_event(QMouseEvent *event)
{
  if (m_map && m_map->handleEvent(event)) {
    event->accept();
    return;
  }

  m_mouse_point.reset(createTouchPointFromMouseEvent(event, QEventPoint::State::Updated));
  if (m_touch_points.isEmpty())
    update();
  event->accept();
}

/// \internal
void QcMapGestureArea::handle_mouse_release_event(QMouseEvent *event)
{
  if (m_map && m_map->handleEvent(event)) {
    event->accept();
    return;
  }

  if (!m_mouse_point.isNull()) {
    //this looks super ugly , however is required in case we do not get synthesized MouseReleaseEvent
    //and we reset the point already in handle_touch_ungrab_event
    m_mouse_point.reset(createTouchPointFromMouseEvent(event, QEventPoint::State::Released));
    if (m_touch_points.isEmpty())
      update();
  }
  event->accept();
}

/// \internal
void QcMapGestureArea::handle_mouse_ungrab_event()
{

  if (m_touch_points.isEmpty() && !m_mouse_point.isNull()) {
    m_mouse_point.reset();
    update();
  } else {
    m_mouse_point.reset();
  }
}

/// \internal
void QcMapGestureArea::handle_touch_ungrab_event()
{
  m_touch_points.clear();
  //this is needed since in some cases mouse release is not delivered
  //(second touch point breaks mouse synthesized events)
  m_mouse_point.reset();
  update();
}

/// \internal
void QcMapGestureArea::handle_touch_event(QTouchEvent *event)
{
  if (m_map && m_map->handleEvent(event)) {
    event->accept();
    return;
  }

  m_touch_points.clear();
  m_mouse_point.reset();

  for (int i = 0; i < event->touch_points().count(); ++i) {
    auto point = event->touch_points().at(i);
    if (point.state() != QEventPoint::State::Released)
      m_touch_points << point;
  }
  if (event->touch_points().count() >= 2)
    event->accept();
  else
    event->ignore();
  update();
}

#if QT_CONFIG(wheelevent)
void QcMapGestureArea::handle_wheel_event(QWheelEvent *event)
{
  if (!m_map)
    return;

  if (m_map->handleEvent(event)) {
    event->accept();
    return;
  }

  const QGeoCoordinate &wheelGeoPos = m_declarative_map->toCoordinate(event->position(), false);
  const QcVectorDouble &preZoomPoint = event->position();

  // Not using AltModifier as, for some reason, it causes angle_delta to be 0
  if (event->modifiers() & Qt::ShiftModifier && rotation_enabled()) {
    emit rotation_started(&m_pinch.m_event);
    // First set bearing
    const double bearingDelta = event->angle_delta().y() * qreal(0.05);
    m_declarative_map->setBearing(m_declarative_map->bearing() + bearingDelta,  wheelGeoPos);
    emit rotation_updated(&m_pinch.m_event);
    emit rotation_finished(&m_pinch.m_event);
  } else if (event->modifiers() & Qt::ControlModifier && tilt_enabled()) {
    emit tilt_started(&m_pinch.m_event);
    const double tiltDelta = event->angle_delta().y() * qreal(0.05);
    m_declarative_map->setTilt(m_declarative_map->tilt() + tiltDelta);
    emit tilt_updated(&m_pinch.m_event);
    emit tilt_finished(&m_pinch.m_event);
  } else if (pinch_enabled()) {
    const double zoomLevelDelta = event->angle_delta().y() * qreal(0.001);
    // Gesture area should always honor maxZL, but Map might not.
    m_declarative_map->setZoomLevel(qMin<qreal>(m_declarative_map->zoomLevel() + zoomLevelDelta, maximum_zoom_level()),
                                    false);
    const QcVectorDouble &postZoomPoint = m_declarative_map->fromCoordinate(wheelGeoPos, false);

    if (preZoomPoint != postZoomPoint) // need to re-anchor the wheel geoPos to the event position
      m_declarative_map->alignCoordinateToPoint(wheelGeoPos, preZoomPoint);
  }
  event->accept();
}
#endif

/// \internal
void QcMapGestureArea::clear_touch_data()
{
  m_flick_vector = QVector2D();
  m_touch_pointsCentroid.setX(0);
  m_touch_pointsCentroid.setY(0);
  m_touch_center_coordinate.setLongitude(0);
  m_touch_center_coordinate.setLatitude(0);
  m_start_coordinate.setLongitude(0);
  m_start_coordinate.setLatitude(0);
}

/// \internal
void QcMapGestureArea::update_flick_parameters(const QcVectorDouble &pos)
{
  // Take velocity samples every sufficient period of time, used later to determine the flick
  // duration and speed (when mouse is released).
  qreal elapsed = qreal(m_last_position_time.elapsed());

  if (elapsed >= QML_MAP_FLICK_VELOCITY_SAMPLE_PERIOD) {
    elapsed /= 1000.;
    qreal vel  = distance_between_touch_points(pos, m_last_position) / elapsed;
    m_flick_vector = (QVector2D(pos) - QVector2D(m_last_position)).normalized();
    m_flick_vector *= qBound<qreal>(-m_flick.m_max_velocity, vel, m_flick.m_max_velocity);

    m_last_position = pos;
    m_last_position_time.restart();
  }
}

void QcMapGestureArea::set_touch_point_state(const QcMapGestureArea::TouchPointState state)
{
  m_touch_point_state = state;
}

void QcMapGestureArea::set_flick_state(const QcMapGestureArea::FlickState state)
{
  m_flick_state = state;
}

void QcMapGestureArea::set_tilt_state(const QcMapGestureArea::TiltState state)
{
  m_tilt_state = state;
}

void QcMapGestureArea::set_rotation_state(const QcMapGestureArea::RotationState state)
{
  m_rotation_state = state;
}

void QcMapGestureArea::set_pinch_state(const QcMapGestureArea::PinchState state)
{
  m_pinch_state = state;
}

/// \internal
bool QcMapGestureArea::is_active() const
{
  return is_pan_active() || is_pinch_active() || is_rotation_active() || is_tilt_active();
}

/// \internal
// simplify the gestures by using a state-machine format (easy to move to a future state machine)
void QcMapGestureArea::update()
{
  if (!m_map)
    return;
  // First state machine is for the number of touch points

  //combine touch with mouse event
  m_all_points.clear();
  m_all_points << m_touch_points;
  if (m_all_points.isEmpty() && !m_mouse_point.isNull())
    m_all_points << *m_mouse_point.data();
  std::sort(m_all_points.begin(), m_all_points.end(), [](const QTouchEvent::TouchPoint &tp1, const QTouchEvent::TouchPoint &tp2) { return tp1.id() < tp2.id(); });

  touch_point_state_machine();

  // Parallel state machine for tilt. Tilt goes first as it blocks anything else, when started.
  // But tilting can also only start if nothing else is active.
  if (is_tilt_active() || m_pinch.m_tilt_enabled)
    tilt_state_machine();

  // Parallel state machine for pinch
  if (is_pinch_active() || m_pinch.m_pinch_enabled)
    pinch_state_machine();

  // Parallel state machine for rotation.
  if (is_rotation_active() || m_pinch.m_rotation_enabled)
    rotation_state_machine();

  // Parallel state machine for pan (since you can pan at the same time as pinching)
  // The stop_pan function ensures that pan stops immediately when disabled,
  // but the is_pan_active() below allows pan continue its current gesture if you disable
  // the whole gesture.
  // Pan goes last because it does reanchoring in update_pan()  which makes the map
  // properly rotate around the touch point centroid.
  if (is_pan_active() || m_flick.m_flick_enabled || m_flick.m_pan_enabled)
    pan_state_machine();
}

/// \internal
void QcMapGestureArea::touch_point_state_machine()
{
  // Transitions:
  switch (m_touch_point_state) {
  case touch_points0:
    if (m_all_points.count() == 1) {
      clear_touch_data();
      start_one_touch_point();
      set_touch_point_state(touch_points1);
    } else if (m_all_points.count() >= 2) {
      clear_touch_data();
      start_two_touch_points();
      set_touch_point_state(touch_points2);
    }
    break;
  case touch_points1:
    if (m_all_points.count() == 0) {
      set_touch_point_state(touch_points0);
    } else if (m_all_points.count() == 2) {
      m_touch_center_coordinate = m_declarative_map->toCoordinate(m_touch_pointsCentroid, false);
      start_two_touch_points();
      set_touch_point_state(touch_points2);
    }
    break;
  case touch_points2:
    if (m_all_points.count() == 0) {
      set_touch_point_state(touch_points0);
    } else if (m_all_points.count() == 1) {
      m_touch_center_coordinate = m_declarative_map->toCoordinate(m_touch_pointsCentroid, false);
      start_one_touch_point();
      set_touch_point_state(touch_points1);
    }
    break;
  };

  // Update
  switch (m_touch_point_state) {
  case touch_points0:
    break; // do nothing if no touch points down
  case touch_points1:
    update_one_touch_point();
    break;
  case touch_points2:
    update_two_touch_points();
    break;
  }
}

/// \internal
void QcMapGestureArea::start_one_touch_point()
{
  m_scene_start_point1 = mapFromScene(m_all_points.at(0).scenePosition());
  m_last_position = m_scene_start_point1;
  m_last_position_time.start();
  QGeoCoordinate startCoord = m_declarative_map->toCoordinate(m_scene_start_point1, false);
  // ensures a smooth transition for panning
  m_start_coordinate.setLongitude(m_start_coordinate.longitude() + startCoord.longitude() -
                                  m_touch_center_coordinate.longitude());
  m_start_coordinate.setLatitude(m_start_coordinate.latitude() + startCoord.latitude() -
                                 m_touch_center_coordinate.latitude());
}

/// \internal
void QcMapGestureArea::update_one_touch_point()
{
  m_touch_pointsCentroid = mapFromScene(m_all_points.at(0).scenePos());
  update_flick_parameters(m_touch_pointsCentroid);
}

/// \internal
void QcMapGestureArea::start_two_touch_points()
{
  m_scene_start_point1 = mapFromScene(m_all_points.at(0).scenePos());
  m_scene_start_point2 = mapFromScene(m_all_points.at(1).scenePos());
  QcVectorDouble startPos = (m_scene_start_point1 + m_scene_start_point2) * 0.5;
  m_last_position = startPos;
  m_last_position_time.start();
  QGeoCoordinate startCoord = m_declarative_map->toCoordinate(startPos, false);
  m_start_coordinate.setLongitude(m_start_coordinate.longitude() + startCoord.longitude() -
                                  m_touch_center_coordinate.longitude());
  m_start_coordinate.setLatitude(m_start_coordinate.latitude() + startCoord.latitude() -
                                 m_touch_center_coordinate.latitude());
  m_two_touch_angle_start = touch_angle(m_scene_start_point1, m_scene_start_point2); // Initial angle used for calculating rotation
  m_distance_between_touch_points_start = distance_between_touch_points(m_scene_start_point1, m_scene_start_point2);
  m_two_touch_points_centroid_start = (m_scene_start_point1 + m_scene_start_point2) / 2;
}

/// \internal
void QcMapGestureArea::update_two_touch_points()
{
  QcVectorDouble p1 = mapFromScene(m_all_points.at(0).scenePos());
  QcVectorDouble p2 = mapFromScene(m_all_points.at(1).scenePos());
  m_distance_between_touch_points = distance_between_touch_points(p1, p2);
  m_touch_pointsCentroid = (p1 + p2) / 2;
  update_flick_parameters(m_touch_pointsCentroid);

  m_two_touch_angle = touch_angle(p1, p2);
}

/// \internal
void QcMapGestureArea::tilt_state_machine()
{
  TiltState lastState = m_tilt_state;
  // Transitions:
  switch (m_tilt_state) {
  case tilt_inactive:
    if (m_all_points.count() >= 2) {
      if (!is_rotation_active() && !is_pinch_active() && can_start_tilt()) { // only gesture that can be overridden: pan/flick
        m_declarative_map->setKeepMouseGrab(true);
        m_declarative_map->setKeepTouchGrab(true);
        start_tilt();
        set_tilt_state(tilt_active);
      } else {
        set_tilt_state(tilt_inactive_two_points);
      }
    }
    break;
  case tilt_inactive_two_points:
    if (m_all_points.count() <= 1) {
      set_tilt_state(tilt_inactive);
    } else {
      if (!is_rotation_active() && !is_pinch_active() && can_start_tilt()) { // only gesture that can be overridden: pan/flick
        m_declarative_map->setKeepMouseGrab(true);
        m_declarative_map->setKeepTouchGrab(true);
        start_tilt();
        set_tilt_state(tilt_active);
      }
    }
    break;
  case tilt_active:
    if (m_all_points.count() <= 1) {
      set_tilt_state(tilt_inactive);
      m_declarative_map->setKeepMouseGrab(m_prevent_stealing);
      m_declarative_map->setKeepTouchGrab(m_prevent_stealing);
      end_tilt();
    }
    break;
  }
  // This line implements an exclusive state machine, where the transitions and updates don't
  // happen on the same frame
  if (m_tilt_state != lastState) {
    emit tilt_activeChanged();
    return;
  }

  // Update
  switch (m_tilt_state) {
  case tilt_inactive:
  case tilt_inactive_two_points:
    break; // do nothing
  case tilt_active:
    update_tilt();
    break;
  }
}

bool validateTouchAngleForTilting(const qreal angle)
{
  return ((qAbs(angle) - 180.0) < MaximumParallelPosition) || (qAbs(angle) < MaximumParallelPosition);
}

/// \internal
bool QcMapGestureArea::can_start_tilt()
{
  if (m_all_points.count() >= 2) {
    QcVectorDouble p1 = mapFromScene(m_all_points.at(0).scenePos());
    QcVectorDouble p2 = mapFromScene(m_all_points.at(1).scenePos());
    if (validateTouchAngleForTilting(m_two_touch_angle)
        && moving_parallel_vertical(m_scene_start_point1, p1, m_scene_start_point2, p2)
        && qAbs(m_two_touch_points_centroid_start.y() - m_touch_pointsCentroid.y()) > MinimumPanToTiltDelta) {
      m_pinch.m_event.set_center(mapFromScene(m_touch_pointsCentroid));
      m_pinch.m_event.set_angle(m_two_touch_angle);
      m_pinch.m_event.set_point1(p1);
      m_pinch.m_event.set_point2(p2);
      m_pinch.m_event.set_number_of_points(m_all_points.count());
      m_pinch.m_event.set_accepted(true);
      emit tilt_started(&m_pinch.m_event);
      return true;
    }
  }
  return false;
}

/// \internal
void QcMapGestureArea::start_tilt()
{
  if (is_pan_active()) {
    stop_pan();
    set_flick_state(flick_inactive);
  }

  m_pinch.m_tilt.m_start_touch_centroid = m_touch_pointsCentroid;
  m_pinch.m_tilt.m_start_tilt = m_declarative_map->tilt();
}

/// \internal
void QcMapGestureArea::update_tilt()
{
  // Calculate the new tilt
  qreal verticalDisplacement = (m_touch_pointsCentroid - m_pinch.m_tilt.m_start_touch_centroid).y();

  // Approach: 10pixel = 1 degree.
  qreal tilt =  verticalDisplacement / 10.0;
  qreal newTilt = m_pinch.m_tilt.m_start_tilt - tilt;
  m_declarative_map->setTilt(newTilt);

  m_pinch.m_event.set_center(mapFromScene(m_touch_pointsCentroid));
  m_pinch.m_event.set_angle(m_two_touch_angle);
  m_pinch.m_last_point1 = mapFromScene(m_all_points.at(0).scenePos());
  m_pinch.m_last_point2 = mapFromScene(m_all_points.at(1).scenePos());
  m_pinch.m_event.set_point1(m_pinch.m_last_point1);
  m_pinch.m_event.set_point2(m_pinch.m_last_point2);
  m_pinch.m_event.set_number_of_points(m_all_points.count());
  m_pinch.m_event.set_accepted(true);

  emit tilt_updated(&m_pinch.m_event);
}

/// \internal
void QcMapGestureArea::end_tilt()
{
  QcVectorDouble p1 = mapFromScene(m_pinch.m_last_point1);
  QcVectorDouble p2 = mapFromScene(m_pinch.m_last_point2);
  m_pinch.m_event.set_center((p1 + p2) / 2);
  m_pinch.m_event.set_angle(m_pinch.m_last_angle);
  m_pinch.m_event.set_point1(p1);
  m_pinch.m_event.set_point2(p2);
  m_pinch.m_event.set_accepted(true);
  m_pinch.m_event.set_number_of_points(0);
  emit tilt_finished(&m_pinch.m_event);
}

/// \internal
void QcMapGestureArea::rotation_state_machine()
{
  RotationState lastState = m_rotation_state;
  // Transitions:
  switch (m_rotation_state) {
  case RotationInactive:
    if (m_all_points.count() >= 2) {
      if (!is_tilt_active() && can_start_rotation()) {
        m_declarative_map->setKeepMouseGrab(true);
        m_declarative_map->setKeepTouchGrab(true);
        start_rotation();
        set_rotation_state(rotation_active);
      } else {
        set_rotation_state(RotationInactiveTwoPoints);
      }
    }
    break;
  case RotationInactiveTwoPoints:
    if (m_all_points.count() <= 1) {
      set_rotation_state(RotationInactive);
    } else {
      if (!is_tilt_active() && can_start_rotation()) {
        m_declarative_map->setKeepMouseGrab(true);
        m_declarative_map->setKeepTouchGrab(true);
        start_rotation();
        set_rotation_state(rotation_active);
      }
    }
    break;
  case rotation_active:
    if (m_all_points.count() <= 1) {
      set_rotation_state(RotationInactive);
      m_declarative_map->setKeepMouseGrab(m_prevent_stealing);
      m_declarative_map->setKeepTouchGrab(m_prevent_stealing);
      end_rotation();
    }
    break;
  }
  // This line implements an exclusive state machine, where the transitions and updates don't
  // happen on the same frame
  if (m_rotation_state != lastState) {
    emit rotation_activeChanged();
    return;
  }

  // Update
  switch (m_rotation_state) {
  case RotationInactive:
  case RotationInactiveTwoPoints:
    break; // do nothing
  case rotation_active:
    update_rotation();
    break;
  }
}

/// \internal
bool QcMapGestureArea::can_start_rotation()
{
  if (m_all_points.count() >= 2) {
    QcVectorDouble p1 = mapFromScene(m_all_points.at(0).scenePos());
    QcVectorDouble p2 = mapFromScene(m_all_points.at(1).scenePos());
    if (point_dragged(m_scene_start_point1, p1) || point_dragged(m_scene_start_point2, p2)) {
      qreal delta = angle_delta(m_two_touch_angle_start, m_two_touch_angle);
      if (qAbs(delta) < MinimumRotationStartingAngle) {
        return false;
      }
      m_pinch.m_event.set_center(mapFromScene(m_touch_pointsCentroid));
      m_pinch.m_event.set_angle(m_two_touch_angle);
      m_pinch.m_event.set_point1(p1);
      m_pinch.m_event.set_point2(p2);
      m_pinch.m_event.set_number_of_points(m_all_points.count());
      m_pinch.m_event.set_accepted(true);
      emit rotation_started(&m_pinch.m_event);
      return m_pinch.m_event.accepted();
    }
  }
  return false;
}

/// \internal
void QcMapGestureArea::start_rotation()
{
  m_pinch.m_rotation.m_start_bearing = m_declarative_map->bearing();
  m_pinch.m_rotation.m_previous_touch_angle = m_two_touch_angle;
  m_pinch.m_rotation.m_total_angle = 0.0;
}

/// \internal
void QcMapGestureArea::update_rotation()
{
  // Calculate the new bearing
  qreal angle = angle_delta(m_pinch.m_rotation.m_previous_touch_angle, m_two_touch_angle);
  if (qAbs(angle) < 0.2) // avoiding too many updates
    return;

  m_pinch.m_rotation.m_previous_touch_angle = m_two_touch_angle;
  m_pinch.m_rotation.m_total_angle += angle;
  qreal newBearing = m_pinch.m_rotation.m_start_bearing - m_pinch.m_rotation.m_total_angle;
  m_declarative_map->setBearing(newBearing);

  m_pinch.m_event.set_center(mapFromScene(m_touch_pointsCentroid));
  m_pinch.m_event.set_angle(m_two_touch_angle);
  m_pinch.m_last_point1 = mapFromScene(m_all_points.at(0).scenePos());
  m_pinch.m_last_point2 = mapFromScene(m_all_points.at(1).scenePos());
  m_pinch.m_event.set_point1(m_pinch.m_last_point1);
  m_pinch.m_event.set_point2(m_pinch.m_last_point2);
  m_pinch.m_event.set_number_of_points(m_all_points.count());
  m_pinch.m_event.set_accepted(true);

  emit rotation_updated(&m_pinch.m_event);
}

/// \internal
void QcMapGestureArea::end_rotation()
{
  QcVectorDouble p1 = mapFromScene(m_pinch.m_last_point1);
  QcVectorDouble p2 = mapFromScene(m_pinch.m_last_point2);
  m_pinch.m_event.set_center((p1 + p2) / 2);
  m_pinch.m_event.set_angle(m_pinch.m_last_angle);
  m_pinch.m_event.set_point1(p1);
  m_pinch.m_event.set_point2(p2);
  m_pinch.m_event.set_accepted(true);
  m_pinch.m_event.set_number_of_points(0);
  emit rotation_finished(&m_pinch.m_event);
}

/// \internal
void QcMapGestureArea::pinch_state_machine()
{
  PinchState lastState = m_pinch_state;
  // Transitions:
  switch (m_pinch_state) {
  case PinchInactive:
    if (m_all_points.count() >= 2) {
      if (!is_tilt_active() && can_start_pinch()) {
        m_declarative_map->setKeepMouseGrab(true);
        m_declarative_map->setKeepTouchGrab(true);
        start_pinch();
        set_pinch_state(pinch_active);
      } else {
        set_pinch_state(PinchInactiveTwoPoints);
      }
    }
    break;
  case PinchInactiveTwoPoints:
    if (m_all_points.count() <= 1) {
      set_pinch_state(PinchInactive);
    } else {
      if (!is_tilt_active() && can_start_pinch()) {
        m_declarative_map->setKeepMouseGrab(true);
        m_declarative_map->setKeepTouchGrab(true);
        start_pinch();
        set_pinch_state(pinch_active);
      }
    }
    break;
  case pinch_active:
    if (m_all_points.count() <= 1) { // Once started, pinch goes off only when finger(s) are release
      set_pinch_state(PinchInactive);
      m_declarative_map->setKeepMouseGrab(m_prevent_stealing);
      m_declarative_map->setKeepTouchGrab(m_prevent_stealing);
      end_pinch();
    }
    break;
  }
  // This line implements an exclusive state machine, where the transitions and updates don't
  // happen on the same frame
  if (m_pinch_state != lastState) {
    emit pinch_activeChanged();
    return;
  }

  // Update
  switch (m_pinch_state) {
  case PinchInactive:
  case PinchInactiveTwoPoints:
    break; // do nothing
  case pinch_active:
    update_pinch();
    break;
  }
}

/// \internal
bool QcMapGestureArea::can_start_pinch()
{
  if (m_all_points.count() >= 2) {
    QcVectorDouble p1 = mapFromScene(m_all_points.at(0).scenePos());
    QcVectorDouble p2 = mapFromScene(m_all_points.at(1).scenePos());
    if (qAbs(m_distance_between_touch_points - m_distance_between_touch_points_start) > MinimumPinchDelta) {
      m_pinch.m_event.set_center(mapFromScene(m_touch_pointsCentroid));
      m_pinch.m_event.set_angle(m_two_touch_angle);
      m_pinch.m_event.set_point1(p1);
      m_pinch.m_event.set_point2(p2);
      m_pinch.m_event.set_number_of_points(m_all_points.count());
      m_pinch.m_event.set_accepted(true);
      emit pinch_started(&m_pinch.m_event);
      return m_pinch.m_event.accepted();
    }
  }
  return false;
}

/// \internal
void QcMapGestureArea::start_pinch()
{
  m_pinch.m_start_distance = m_distance_between_touch_points;
  m_pinch.m_zoom.m_previous = m_declarative_map->zoomLevel();
  m_pinch.m_last_angle = m_two_touch_angle;

  m_pinch.m_last_point1 = mapFromScene(m_all_points.at(0).scenePos());
  m_pinch.m_last_point2 = mapFromScene(m_all_points.at(1).scenePos());

  m_pinch.m_zoom.m_start = m_declarative_map->zoomLevel();
}

/// \internal
void QcMapGestureArea::update_pinch()
{
  // Calculate the new zoom level if we have distance ( >= 2 touchpoints), otherwise stick with old.
  qreal newZoomLevel = m_pinch.m_zoom.m_previous;
  if (m_distance_between_touch_points) {
    newZoomLevel =
      // How much further/closer the current touchpoints are (in pixels) compared to pinch start
      ((m_distance_between_touch_points - m_pinch.m_start_distance)  *
       //  How much one pixel corresponds in units of zoomlevel (and multiply by above delta)
       (m_pinch.m_zoom.maximum_change / ((width() + height()) / 2))) +
      // Add to starting zoom level. Sign of (dist-pinchstartdist) takes care of zoom in / out
      m_pinch.m_zoom.m_start;
  }

  m_pinch.m_event.set_center(mapFromScene(m_touch_pointsCentroid));
  m_pinch.m_event.set_angle(m_two_touch_angle);

  m_pinch.m_last_point1 = mapFromScene(m_all_points.at(0).scenePos());
  m_pinch.m_last_point2 = mapFromScene(m_all_points.at(1).scenePos());
  m_pinch.m_event.set_point1(m_pinch.m_last_point1);
  m_pinch.m_event.set_point2(m_pinch.m_last_point2);
  m_pinch.m_event.set_number_of_points(m_all_points.count());
  m_pinch.m_event.set_accepted(true);

  m_pinch.m_last_angle = m_two_touch_angle;
  emit pinch_updated(&m_pinch.m_event);

  if (m_accepted_gestures & PinchGesture) {
    // Take maximum and minimumzoomlevel into account
    qreal perPinchMinimumZoomLevel = qMax(m_pinch.m_zoom.m_start - m_pinch.m_zoom.maximum_change, m_pinch.m_zoom.m_minimum);
    qreal perPinchMaximumZoomLevel = qMin(m_pinch.m_zoom.m_start + m_pinch.m_zoom.maximum_change, m_pinch.m_zoom.m_maximum);
    newZoomLevel = qMin(qMax(perPinchMinimumZoomLevel, newZoomLevel), perPinchMaximumZoomLevel);
    m_declarative_map->setZoomLevel(qMin<qreal>(newZoomLevel, maximum_zoom_level()), false);
    m_pinch.m_zoom.m_previous = newZoomLevel;
  }
}

/// \internal
void QcMapGestureArea::end_pinch()
{
  QcVectorDouble p1 = mapFromScene(m_pinch.m_last_point1);
  QcVectorDouble p2 = mapFromScene(m_pinch.m_last_point2);
  m_pinch.m_event.set_center((p1 + p2) / 2);
  m_pinch.m_event.set_angle(m_pinch.m_last_angle);
  m_pinch.m_event.set_point1(p1);
  m_pinch.m_event.set_point2(p2);
  m_pinch.m_event.set_accepted(true);
  m_pinch.m_event.set_number_of_points(0);
  emit pinch_finished(&m_pinch.m_event);
  m_pinch.m_start_distance = 0;
}

/// \internal
void QcMapGestureArea::pan_state_machine()
{
  FlickState lastState = m_flick_state;

  // Transitions
  switch (m_flick_state) {
  case flick_inactive:
    if (!is_tilt_active() && can_start_pan()) {
      // Update startCoord_ to ensure smooth start for panning when going over start_drag_distance
      QGeoCoordinate newStartCoord = m_declarative_map->toCoordinate(m_touch_pointsCentroid, false);
      m_start_coordinate.setLongitude(newStartCoord.longitude());
      m_start_coordinate.setLatitude(newStartCoord.latitude());
      m_declarative_map->setKeepMouseGrab(true);
      set_flick_state(pan_active);
    }
    break;
  case pan_active:
    if (m_all_points.count() == 0) {
      if (!try_start_flick())
        {
          set_flick_state(flick_inactive);
          // mark as inactive for use by camera
          if (m_pinch_state == PinchInactive && m_rotation_state == RotationInactive && m_tilt_state == tilt_inactive) {
            m_declarative_map->setKeepMouseGrab(m_prevent_stealing);
            m_map->prefetchData();
          }
          emit pan_finished();
        } else {
        set_flick_state(flick_active);
        emit pan_finished();
        emit flick_started();
      }
    }
    break;
  case flick_active:
    if (m_all_points.count() > 0) { // re touched before movement ended
      stop_flick();
      m_declarative_map->setKeepMouseGrab(true);
      set_flick_state(pan_active);
    }
    break;
  }

  if (m_flick_state != lastState)
    emit pan_activeChanged();

  // Update
  switch (m_flick_state) {
  case flick_inactive: // do nothing
    break;
  case pan_active:
    update_pan();
    // this ensures 'pan_started' occurs after the pan has actually started
    if (lastState != pan_active)
      emit pan_started();
    break;
  case flick_active:
    break;
  }
}
/// \internal
bool QcMapGestureArea::can_start_pan()
{
  if (m_all_points.count() == 0 || (m_accepted_gestures & PanGesture) == 0
      || (m_mouse_point && m_mouse_point->state() == QEventPoint::State::Released)) // mouseReleaseEvent handling does not clear m_mouse_point, only ungrabMouse does -- QTBUG-66534
    return false;

  // Check if thresholds for normal panning are met.
  // (normal panning vs flicking: flicking will start from mouse release event).
  const int start_drag_distance = qApp->styleHints()->start_drag_distance() * 2;
  QcVectorDouble p1 = mapFromScene(m_all_points.at(0).scenePos());
  int dyFromPress = int(p1.y() - m_scene_start_point1.y());
  int dxFromPress = int(p1.x() - m_scene_start_point1.x());
  if ((qAbs(dyFromPress) >= start_drag_distance || qAbs(dxFromPress) >= start_drag_distance))
    return true;
  return false;
}

/// \internal
void QcMapGestureArea::update_pan()
{
  m_declarative_map->alignCoordinateToPoint(m_start_coordinate, m_touch_pointsCentroid);
}

/// \internal
bool QcMapGestureArea::try_start_flick()
{
  if ((m_accepted_gestures & FlickGesture) == 0)
    return false;
  // if we drag then pause before release we should not cause a flick.
  qreal flickSpeed = 0.0;
  if (m_last_position_time.elapsed() < QML_MAP_FLICK_VELOCITY_SAMPLE_PERIOD)
    flickSpeed = m_flick_vector.length();

  int flickTime = 0;
  int flickPixels = 0;
  QVector2D flickVector;

  if (qAbs(flickSpeed) > MinimumFlickVelocity
      && distance_between_touch_points(m_touch_pointsCentroid, m_scene_start_point1) > FlickThreshold) {
    qreal acceleration = m_flick.m_deceleration;
    if ((flickSpeed > 0.0f) == (m_flick.m_deceleration > 0.0f))
      acceleration = acceleration * -1.0f;
    flickTime = static_cast<int>(-1000 * flickSpeed / acceleration);
    flickPixels = (flickTime * flickSpeed) / 2000.0;
    flickVector = m_flick_vector.normalized() * flickPixels;
  }

  if (flickTime > 0) {
    start_flick(flickVector.x(), flickVector.y(), flickTime);
    return true;
  }
  return false;
}

/// \internal
void QcMapGestureArea::start_flick(int dx, int dy, int time_ms)
{
  if (!m_flick.m_animation)
    return;
  if (time_ms < 0)
    return;

  QGeoCoordinate animationStartCoordinate = m_declarative_map->center();

  if (m_flick.m_animation->isRunning())
    m_flick.m_animation->stop();
  QGeoCoordinate animationEndCoordinate = m_declarative_map->center();
  m_flick.m_animation->setDuration(time_ms);

  QcVectorDouble delta(dx, dy);
  QMatrix4x4 matBearing;
  matBearing.rotate(m_map->cameraData().bearing(), 0, 0, 1);
  delta = matBearing * delta;

  double zoom = pow(2.0, m_declarative_map->zoomLevel());
  double longitude = animationStartCoordinate.longitude() - (delta.x() / zoom);
  double latitude = animationStartCoordinate.latitude() + (delta.y() / zoom);

  if (delta.x() > 0)
    m_flick.m_animation->setDirection(QcGeoCoordinateAnimation::East);
  else
    m_flick.m_animation->setDirection(QcGeoCoordinateAnimation::West);

  // keep animation in correct bounds
  animationEndCoordinate.setLongitude(QLocationUtils::wrapLong(longitude));
  animationEndCoordinate.setLatitude(QLocationUtils::clipLat(latitude, QLocationUtils::mercatorMaxLatitude()));

  m_flick.m_animation->setFrom(animationStartCoordinate);
  m_flick.m_animation->setTo(animationEndCoordinate);
  m_flick.m_animation->start();
}

void QcMapGestureArea::stop_pan()
{
  if (m_flick_state == flick_active) {
    stop_flick();
  } else if (m_flick_state == pan_active) {
    m_flick_vector = QVector2D();
    set_flick_state(flick_inactive);
    m_declarative_map->setKeepMouseGrab(m_prevent_stealing);
    emit pan_finished();
    emit pan_activeChanged();
    m_map->prefetchData();
  }
}

/// \internal
void QcMapGestureArea::stop_flick()
{
  if (!m_flick.m_animation)
    return;
  m_flick_vector = QVector2D();
  if (m_flick.m_animation->isRunning())
    m_flick.m_animation->stop();
  else
    handle_flick_animation_stopped();
}

void QcMapGestureArea::handle_flick_animation_stopped()
{
  m_declarative_map->setKeepMouseGrab(m_prevent_stealing);
  if (m_flick_state == flick_active) {
    set_flick_state(flick_inactive);
    emit flick_finished();
    emit pan_activeChanged();
    m_map->prefetchData();
  }
}

QT_END_NAMESPACE
