<?php

/**
 * @file
 * Page callbacks for the Simple Reviews Page module.
 */

/**
 * Form constructor for the site-wide reviews form.
 *
 * @see simple_reviews_page_site_form_validate()
 * @see simple_reviews_page_site_form_submit()
 * @ingroup forms
 */
function simple_reviews_page_site_form($form, &$form_state) {
    global $user;

    if (!$user->uid) {
        $form['#attached']['library'][] = array('system', 'jquery.cookie');
        $form['#attributes']['class'][] = 'user-info-from-cookie';
    }

    $form['#attributes']['class'][] = 'simple-reviews-page-form';

    $form['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Your name'),
        '#maxlength' => 50,
        '#default_value' => $user->uid ? format_username($user) : '',
        '#required' => TRUE,
    );
    $form['mail'] = array(
        '#type' => 'textfield',
        '#title' => t('Your e-mail address'),
        '#maxlength' => 100,
        '#default_value' => $user->uid ? $user->mail : '',
        '#required' => TRUE,
    );
    $form['phone'] = array(
        '#type' => 'textfield',
        '#title' => t('Your phone'),
        '#maxlength' => 25,
        '#default_value' => '',
        '#required' => FALSE,
    );
    $form['subject'] = array(
        '#type' => 'textfield',
        '#title' => t('Subject'),
        '#maxlength' => 255,
        '#required' => TRUE,
    );
    $form['message'] = array(
        '#type' => 'textarea',
        '#title' => t('Message'),
        '#required' => TRUE,
    );
    // We do not allow anonymous users to send themselves a copy
    // because it can be abused to spam people.
    $form['copy'] = array(
        '#type' => 'checkbox',
        '#title' => t('Send yourself a copy.'),
        '#access' => $user->uid,
    );
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Send review'),
    );

    return $form;
}

/**
 * Form validation handler for contact_site_form().
 *
 * @see contact_site_form_submit()
 */
function simple_reviews_page_site_form_validate($form, &$form_state) {
    if (!valid_email_address($form_state['values']['mail'])) {
        form_set_error('mail', t('You must enter a valid e-mail address.'));
    }
}

/**
 * Form submission handler for contact_site_form().
 *
 * @see contact_site_form_validate()
 */
function simple_reviews_page_site_form_submit($form, &$form_state) {
    global $user, $language;

    $values = $form_state['values'];
    $values['sender'] = clone $user;
    $values['sender']->name = $values['name'];
    $values['sender']->mail = $values['mail'];

    $rid = db_insert('simple_reviews_page_reviews')
        ->fields(array(
            'name' => $values['name'],
            'email' => $values['mail'],
            'phone' => $values['phone'],
            'topic' => $values['subject'],
            'message' => $values['message'],
            'created' => time(),
        ))
        ->execute();

    // Save the anonymous user information to a cookie for reuse.
    if (!$user->uid) {
        user_cookie_save(array_intersect_key($values, array_flip(array('name', 'mail'))));
    }

    // Get the to and from e-mail addresses.
    //TODO: get email adress from admin panel
    $to = 'qwertggf@mail.ru';
    $from = $values['sender']->mail;

    // Send the e-mail to the recipients using the site default language.
    drupal_mail('simple_reviews_page', 'page_mail', $to, language_default(), $values, $from);

    // Jump to home page rather than back to contact page to avoid
    // contradictory messages if flood control has been activated.
    drupal_set_message(t('Your message has been sent. '. $rid));
    $form_state['redirect'] = '';
}