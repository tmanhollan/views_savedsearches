<?php

/**
 * Implementation of hook_notifications()
 */
function notifications_views_savedsearches_notifications($op, $arg0, $arg1 = NULL, $arg2 = NULL) {
	switch ($op) {
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
    case 'object types':
    	$types['views_savedsearches'] = array(
        'name' => t('Views saved search'),
        'key' => 'sid',
        'load callback' => 'views_savedsearches_load',
        'format callback' => 'notifications_views_savedsearches_2title',
        'access' => 'subscribe to views savedsearch',
    	);
    	return $types;
    case 'event types':
      $types['viewsnewitem'] = array(
        'type' => 'viewsnewitem',
        'action' => 'new',
        'template' => 'notifications-event-views-savedsearches',
        'name' => 'Views Saved Search: [savedsearches-title]',
        'description' => 'Views Savedsearches',
        'line' => "The Views Saved Search [savedsearches-title] has been updated\n[views-arrived-items]",
        'digest' => array('feed', 'type'),
      );
      return $types;
    case 'event actions':
    	return array('new' => t("Content of saved search has changed."));
	}
}

/**
 * Implementation of hook_notifications_event()
 */
function notifications_views_savedsearches_notifications_event($op, $event, $account = NULL) {
	if ($op == 'query' && $event->type == 'viewsnewitem' && $event->action == 'new') {
		$query[]['fields'] = array('sid' => $event->params['sid']);
		return $query;
	}
	if ($op == 'build') {
	}
}

/**
 * Implementation of hook_notifications_subscription()
 */
function notifications_views_savedsearches_notifications_subscription($op, $subscription = NULL, $account = NULL) {
	drupal_set_message("notifications_views_savedsearches_notifications_subscription $op");
	switch ($op) {
		case 'access':
      if (($conditions = $subscription->get_conditions()) && !empty($conditions['sid'])) {
        // Deny access if the sid doesn't exist
        if (!db_result(db_query("SELECT count(*) FROM {views_savedsearches} WHERE sid = %d", $conditions['sid']))) {
          return FALSE;
        }
      }
      break;
	}
}

/**
 * Implementation of hook_notifications_templates().
 */
function notifications_views_savedsearches_notifications_templates($op, $type = 'all', $language = NULL) {
  switch ($op) {
    case 'help':
      if (strpos($type, 'notifications-event') === 0) {
        $help[] = t('The <em>Header</em> and <em>Footer</em> will be taken from Notification events.');
        $help[] = t('The <em>Digest line</em> will be used when composing Short digests on which each event will be just a line.');
        return $help;
      }
      break;
    case 'info':
      $info = array();

      if ($type == 'all' || strpos($type, 'notifications-event-views-savedsearches') === 0) {
        $info['notifications-event-views-savedsearches'] = array(
          'module' => 'notifications_views_savedsearches',
          'name' => t('Notifications for new item in views saved search'),
          'description' => t('Notifications produced when a node is in a views saved search.'),
          'fallback' => 'notifications-event',
        );
      }
      return $info;

    case 'parts':
      switch ($type) {
        case 'notifications-event-views-savedsearches':
        case 'notifications_views_savedsearches-event-viewsnewitem-new':
          // Some parts will be re-used from 'notifications-event' group
          // So we specify only subject and main message
          return array(
            'subject' => t('Subject'),
            'main' => t('Content'),
            'digest' => t('Digest line'),
          );
      }
      break;
    case 'defaults':
      // Event notifications
      switch ($type) {
        case 'notifications-event-views-savedsearches':
        case 'notifications_views_savedsearches-event-viewsnewitem-new':
          return array(
            'subject' => t('Saved search updated: [savedsearches-title]'),
            'main' => array(
              t('There are new items in the views:'), '[views-items]',
            ),
            'digest' => t('New content for saved search [savedsearches-title].'),
          );
      }
      break;
  }
}
