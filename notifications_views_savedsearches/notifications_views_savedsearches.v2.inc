<?php

/**
 * Implementation of hook_notifications().
 */
function notifications_views_savedsearches_notifications($op, &$arg0, $arg1 = NULL, $arg2 = NULL) {
  switch ($op) {
    case 'names':
      $subs = &$arg0;
      if ($subs->event_type == 'savedsearch') {
        if (!empty($subs->fields['sid'])) {
          $data = db_fetch_row(db_query("SELECT name, view_name FROM {views_savedsearches} WHERE sid = %d", $subs->fields['sid']));
          $subs->names['sid'] = t('Views Saved Search: %name, belongs to: %view', array('%name' => $data['name'], '%view' => $data['view_name']));
        }
      }
      break;
    case 'subscription types':
      $types['savedsearch'] = array(
        'title' => t('Views Savedsearch'),
        'access' => 'subscribe to views savedsearch',
        'fields' => array('sid'),
        'event_type' => 'viewsnewitem',
        'description' => t('Subscribe to incoming new items to the given views.'),
      );
      return $types;
    case 'subscription fields':
    // Information about available fields for subscriptions
      $fields['sid'] = array(
        'name' => t('Views Savedsearch'),
        'field' => 'sid',
        'type' => 'int',
        'format callback' => 'notifications_views_savedsearches_2title',
      );
      return $fields;
    case 'event types':
      $types[] = array(
        'type' => 'viewsnewitem',
        'action' => 'new',
        'name' => 'Views Saved Search: [savedsearches-title]',
        'line' => "The Views Saved Search [savedsearches-title] has been updated\n[views-arrived-items]",
        'digest' => array('feed', 'type'),
      );
      return $types;
    case 'query':
    // $arg2 is $event array.
      if ($arg0 == 'event' && $arg1 == 'viewsnewitem' && ($vs = $arg2->subscription)) {
        $query[]['fields'] = array(
          'sid' => $vs->sid,
        );
        return $query;
      }
      break;
    case 'event objects':
      return array('viewsnewitem' => t('Views Saved Searches'));
    case 'event load':
      $event = &$arg0;
      if ($event->type == 'viewsnewitem') {
        if (!empty($event->params['sid'])) {
          $event->savedsearch = views_savedsearches_load($event->params['sid']);
        }
      }
      break;
  }
}

/**
 * Implementation of hook_messaging().
 */
function notifications_views_savedsearches_messaging($op, $type = NULL, $arg2 = NULL, $arg3 = NULL, $arg4 = NULL) {
  switch ($op) {
    case 'message groups':
    // Generic notifications event
      $info['notifications-event-viewsnewitem'] = array(
        'module' => 'notifications_views_savedsearches',
        'name' => t('Notifications for views saved searches events'),
        'fallback' => 'notifications-event',
      );
      return $info;
    case 'message keys':
      switch ($type) {
        case 'notifications-event-viewsnewitem':
        // Some parts will be re-used from 'notifications-event' group
        // So we specify only subject and main message
          return array(
            'subject' => t('Subject'),
            'main' => t('Content'),
            'digest' => t('Digest line'),
          );
          break;
      }
      break;
    case 'messages':
    // Event notifications
      switch ($type) {
        case 'notifications-event-viewsnewitem':
          return array(
            'subject' => t('Saved search updated: [savedsearches-title]'),
            'main' => array(
              t('There are new items in the views:'), '[views-items]',
            ),
            'digest' => t('New content for saved search [savedsearches-title].'),
          );
      }
      break;
    case 'tokens':
      if ($type == 'notifications-event-viewsnewitem') {
        return array('viewsnewitem');
      }
      break;
  }
}
