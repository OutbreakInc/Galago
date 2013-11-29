var logiblockSettings =
{
	namingRules: {},
	locationOverride: undefined
};

$(function($)
{
	//	temp
	/*var $topNav = $("ul.navbar-nav li");

	$topNav.click(function(e)
	{
		var $e = $(e.currentTarget), href = $(e.target).attr("href");
		
		$topNav.removeClass("active");
		$e.addClass("active");
		
		console.log("nav to: " + href);

		e.preventDefault();
		return(false);
	});
	*///	/temp

	//animation routine
	$.fn.animateActive = function animateActive(period)
	{
		var $t = this, i = 0, interval = setInterval(function animateActive_onTimer()
		{
			$t;
			$t.removeClass("active");
			$($t[i]).addClass("active");
			if(++i >= $t.length)	i = 0;
		}, period || 1000);

		return({$ths: $t, stop: function(){clearInterval(interval);}});
	}

	$(".animateActive").children().animateActive(1500);


	function toTitleCase(word)
	{
		if(logiblockSettings && logiblockSettings.namingRules[word])	return(logiblockSettings.namingRules[word]);
		return(word.substr(0, 1).toUpperCase() + word.substr(1));
	}

	//auto-set the active navbar nav item, set the title and show breadcrumbs if necessary
	!(function()
	{
		var path = logiblockSettings.locationOverride || (window && window.location && window.location.pathname) || "";
		var navs = [], path = path.split("/").filter(function(o){return(o != "")});

		var $header = $(".section-header"), $crumbs = $("ol.breadcrumb", $header), $title = $("h1", $header);

		if(path.length > 0)
		{
			$title.html(toTitleCase(path[path.length - 1]));
			$crumbs.append($('<li><a href="/">Home</a></li>'));
			for(var i = 0; i < path.length; i++)
				$crumbs.append(		$((i == (path.length - 1))?
										('<li>' + toTitleCase(path[i]) + '</li>')
										: ('<li><a href="/' + path.slice(0, i + 1).join("/") + '">' + toTitleCase(path[i]) + '</a></li>')
									)
								);
			$header.removeClass("hide");
		}

		$(".navbar ul.navbar-nav li").each(function(i, el)
		{
			var $el = $(el);
			navs.push(
			{
				$el: $el,
				href: $("a", $el).attr("href").split("/").filter(function(o){return(o != "")}).join("/")
			});
		}).removeClass("active");

		//from most-specific to least-specific, compare each navigation link to the current URL and pick the best match
		for(var i = path.length; i >= 0; i--)
		{
			var href = path.slice(0, i).join("/");
			for(var n = 0; n < navs.length; n++)
				if(navs[n].href == href)
					return(navs[n].$el.addClass("active"));
		}
	})();

});

function checkLinks()
{
	$("a").each(function(i, o)
	{
		$.ajax(o.href).fail(function(x, reason, e)
		{
			$(o).css("color", "red");
			$(o).tooltip({title: reason + ", " + x.status + ": " + x.statusText});
		});
	});
}




//payment related
function luhn(s)
{
	var t = 0;
	for(var i = 0; i < 16; i++)
	{
		var d = parseInt(s[i]) * (i & 1)? 1 : 2;
		t += (d > 9)? (parseInt(d / 10) + (d % 10)) : d;
	}
	return((t % 10) == 0);
}
