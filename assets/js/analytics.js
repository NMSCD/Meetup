function track(t,e){posthog.capture(t,{property:e})}!function(t,e){var o,r,p,a;e.__SV||(window.posthog=e,e._i=[],e.init=function(n,s,i){function c(t,e){var o=e.split(".");2==o.length&&(t=t[o[0]],e=o[1]),t[e]=function(){t.push([e].concat(Array.prototype.slice.call(arguments,0)))}}(p=t.createElement("script")).type="text/javascript",p.async=!0,p.src=s.api_host+"/static/array.js",(a=t.getElementsByTagName("script")[0]).parentNode.insertBefore(p,a);var u=e;for(void 0!==i?u=e[i]=[]:i="posthog",u.people=u.people||[],u.toString=function(t){var e="posthog";return"posthog"!==i&&(e+="."+i),t||(e+=" (stub)"),e},u.people.toString=function(){return u.toString(1)+".people (stub)"},o="capture identify alias people.set people.set_once set_config register register_once unregister opt_out_capturing has_opted_out_capturing opt_in_capturing reset isFeatureEnabled onFeatureFlags getFeatureFlag getFeatureFlagPayload reloadFeatureFlags group updateEarlyAccessFeatureEnrollment getEarlyAccessFeatures getActiveMatchingSurveys getSurveys".split(" "),r=0;r<o.length;r++)c(u,o[r]);e._i.push([n,s,i])},e.__SV=1)}(document,window.posthog||[]),posthog.init("phc_oVlYFdN3pKkveI315lJHcZfUbYNBQ2JqA9BqUENCukk",{api_host:"https://eu.posthog.com"});
