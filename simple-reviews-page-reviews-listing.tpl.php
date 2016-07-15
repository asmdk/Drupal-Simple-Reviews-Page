<div id="simple-reviews-page">
    <?php if (!empty($reviews)) : ?>
        <?php foreach ($reviews as $review) : ?>
            <div class="review-content">
                <div class="review">
                    <div><?php echo $review->name; ?><?php echo format_date($review->created); ?></div>
                    <div><?php echo $review->topic; ?></div>
                    <div><?php echo $review->message; ?></div>
                </div>
                <blockquote class="admin-comment">
                    <div><?php echo $review->c_message; ?></div>
                    <div><?php echo $review->c_name; ?><?php echo format_date($review->c_changed); ?></div>
                </blockquote>
            </div>
        <?php endforeach; ?>
    <?php endif; ?>
</div>
