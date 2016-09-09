<div id="simple-reviews-page">
    <div class="leave-feedback"><a href="<?php echo url( 'reviews/send'); ?>"><?php echo t('Leave feedback') ;?></a></div>
    <?php if (!empty($reviews)) : ?>
        <?php foreach ($reviews as $index => $review) : ?>
            <div class="review-content<?php if ($index == 0) echo ' first'; if ($index == (count($reviews) - 1)) echo ' last'; ?>">
                <div class="review">
                    <div class="question-name">
                        <?php echo $review->name; ?> <span><?php echo format_date($review->created, 'short'); ?></span>
                    </div>
                    <h3 class="question-topic"><?php echo $review->topic; ?></h3>
                    <div class="question-message"><?php echo $review->message; ?></div>
                </div>
                <?php if (!empty($review->c_message)) : ?>
                    <div class="admin-comment">
                        <div class="answer-name">
                            <?php echo $review->c_name; ?>
                            <span>(<?php echo $review->c_office; ?>)</span>:
                        </div>
                        <div class="answer-message"><?php echo $review->c_message; ?></div>
                    </div>
                <?php endif; ?>
            </div>
        <?php endforeach; ?>
        <div class="leave-feedback"><a href="<?php echo url( 'reviews/send'); ?>"><?php echo t('Leave feedback') ;?></a></div>
    <?php endif; ?>
</div>
