module.exports = function (context, options) {
    if (context == null) return '/';
    if (context.includes('?')) {
        return context + '&amp;ref=assistantNMS';
    }
    return context + '?ref=assistantNMS';
};