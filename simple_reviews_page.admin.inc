<?php

/**
 * @file
 * Admin page callbacks for the Simple Reviews Page module.
 */

/**
 * Categories/list tab.
 */
function simple_reviews_page_list() {
    $header = array(
        t('Name'),
        t('Email'),
        t('Phone'),
        t('Subject'),
        t('Message'),
        t('Status'),
        t('Date'),
        t('Order'),
        array('data' => t('Operations'), 'colspan' => 2),
    );
    $rows = array();

    // Get all the reviews from the database.
    $reviews = db_select('simple_reviews_page_reviews', 'r')
        ->addTag('translatable')
        ->fields('r', array('rid', 'name', 'email', 'phone', 'topic', 'message', 'status', 'created', 'sticky'))
        ->orderBy('created', 'DESC')
        ->orderBy('sticky', 'DESC')
        ->execute()
        ->fetchAll();

    // Loop through the reviews and add them to the table.
    foreach ($reviews as $review) {
        $rows[] = array(
            $review->name,
            $review->email,
            $review->phone,
            check_plain($review->topic),
            check_plain($review->message),
            ($review->status ? t('Active') : t('Inactive')),
            format_date($review->created, 'short'),
            $review->sticky,
            l(t('Edit'), 'admin/structure/simple_reviews_page/edit/' . $review->rid),
            l(t('Delete'), 'admin/structure/simple_reviews_page/delete/' . $review->rid),
        );
    }

    if (!$rows) {
        $rows[] = array(array(
            'data' => t('No reviews available.'),
            'colspan' => 9,
        ));
    }

    $build['category_table'] = array(
        '#theme' => 'table',
        '#header' => $header,
        '#rows' => $rows,
    );
    return $build;
}

/**
 * Form constructor for the category edit form.
 *
 * @param $review
 *   An array describing the category to be edited. May be empty for new
 *   categories. Recognized array keys are:
 *   - category: The name of the category.
 *   - recipients: A comma-separated list of recipients.
 *   - reply: (optional) The body of the auto-reply message.
 *   - weight: The weight of the category.
 *   - selected: Boolean indicating whether the category should be selected by
 *     default.
 *   - cid: The category ID for which the form is to be displayed.
 *
 * @see contact_category_edit_form_validate()
 * @see contact_category_edit_form_submit()
 * @ingroup forms
 * @return
 */
function simple_reviews_page_edit_form($form, &$form_state, array $review = array()) {
    // If this is a new category, add the default values.
    $review += array(
        'name' => '',
        'email' => '',
        'phone' => null,
        'subject' => '',
        'message' => '',
        'status' => 0,
        'date' => null,
        'sticky' => 0,
        'rid' => null,
    );

    $form['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Name'),
        '#maxlength' => 50,
        '#default_value' => $review['name'],
        '#required' => TRUE,
    );
    $form['email'] = array(
        '#type' => 'textfield',
        '#maxlength' => 100,
        '#title' => t('Email'),
        '#default_value' => $review['email'],
        '#required' => TRUE,
    );
    $form['phone'] = array(
        '#type' => 'textfield',
        '#maxlength' => 25,
        '#title' => t('Phone'),
        '#default_value' => $review['phone'],
    );
    $form['subject'] = array(
        '#type' => 'textfield',
        '#maxlength' => 255,
        '#title' => t('Subject'),
        '#default_value' => $review['subject'],
    );
    $form['message'] = array(
        '#type' => 'textarea',
        '#title' => t('Message'),
        '#default_value' => $review['message'],
    );
    $form['status'] = array(
        '#type' => 'select',
        '#title' => t('Status'),
        '#options' => array(
            0 => t('Inactive'),
            1 => t('Active'),
        ),
        '#default_value' => $review['status'],
    );
    $form['sticky'] = array(
        '#type' => 'textfield',
        '#maxlength' => 10,
        '#title' => t('Order'),
        '#default_value' => $review['sticky'],
    );
    $form['cid'] = array(
        '#type' => 'value',
        '#value' => $review['rid'],
    );
    $form['date'] = array(
        '#type' => 'value',
        '#value' => $review['date'],
    );
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );

    return $form;
}

/**
 * Form validation handler for simple_reviews_page_edit_form().
 *
 * @see simple_reviews_page_edit_form_submit()
 */
function simple_reviews_page_edit_form_validate($form, &$form_state) {
    $email = trim($form_state['values']['email']);
    if (!valid_email_address($email)) {
        form_set_error('email', t('%email is an invalid e-mail address.', array('%email' => $email)));
    }

    $form_state['values']['email'] = $email;
}

/**
 * Form submission handler for simple_reviews_page_edit_form().
 *
 * @see simple_reviews_page_edit_form_validate()
 */
function simple_reviews_page_edit_form_submit($form, &$form_state) {

    $primaryKeys = empty($form_state['values']['rid']) ? array() : array('rid');
    drupal_write_record('simple_reviews_page_reviews', $form_state['values'], $primaryKeys);


    drupal_set_message(t('Review %review has been saved.', array('%review' => $form_state['values']['simple_reviews_page'])));
    watchdog('simple_reviews_page_reviews', 'Review %review has been saved.', array('%review' => $form_state['values']['simple_reviews_page']), WATCHDOG_NOTICE, l(t('Edit'), 'admin/structure/simple_reviews_page/edit/' . $form_state['values']['rid']));
    $form_state['redirect'] = 'admin/structure/simple_reviews_page';
}

/**
 * Form constructor for the review deletion form.
 *
 * @param $review
 *   Array describing the review to be deleted. See the documentation
 *   of simple_reviews_page_edit_form() for the recognized keys.
 *
 * @see simple_reviews_page_menu()
 * @see simple_reviews_page_delete_form_submit()
 * @return
 */
function simple_reviews_page_delete_form($form, &$form_state, array $review) {
    $form['review'] = array(
        '#type' => 'value',
        '#value' => $review,
    );

    return confirm_form(
        $form,
        t('Are you sure you want to delete %review?', array('%review' => $review['simple_reviews_page'])),
        'admin/structure/simple_reviews_page',
        t('This action cannot be undone.'),
        t('Delete'),
        t('Cancel')
    );
}

/**
 * Form submission handler for simple_reviews_page_delete_form().
 */
function simple_reviews_page_delete_form_submit($form, &$form_state) {
    $contact = $form['simple_reviews_page']['#value'];

    db_delete('simple_reviews_page_reviews')
        ->condition('rid', $contact['rid'])
        ->execute();

    drupal_set_message(t('Review %review has been deleted.', array('%review' => $contact['simple_reviews_page'])));
    watchdog('simple_reviews_page', 'Review %review has been deleted.', array('%review' => $contact['simple_reviews_page']), WATCHDOG_NOTICE);

    $form_state['redirect'] = 'admin/structure/simple_reviews_page';
}
