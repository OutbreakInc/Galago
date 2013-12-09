jQuery.then = function(fn, ctx)
{
	(jQuery.__pr || (jQuery.__pr = [])).push({f: fn, c: ctx});
	return(jQuery);
};
jQuery.resolve = function()
{
	var r = (jQuery.__pr || []);
	jQuery.__pr = [];
	for(var i = 0; i < r.length; i++)
		try			{ r[i].f.apply(r[i].ctx, [jQuery].concat(Array.prototype.slice.call(arguments))); }
		catch(e)	{ setTimeout(function(){throw e;}, 0); }
};



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


//creates an API method that has the effective signature:
//create("POST", "//api.whatever.com/v1", "foo/:barID/baz")
//	-> method(barID, [postJSON], callback, [context]);
//create("GET", "/user/:userID/faves/:faveID")
//	-> method(userID, faveID, [queryParams], callback, [context]);
//callbacks are function(err, data) with this = context
function DaggerAPIMethod(method, urlBase, urlScheme)
{
	var urlParts = urlScheme
					.split("/").filter(function(p){
							return(p !== "");
						})
					.map(function(p){
							return((p.substr(0, 1) === ":")? null : p);
						});
	
	if(urlScheme === undefined)
	{
		urlScheme = urlBase;
		urlBase = "";
	}

	return(function()
	{
		var argIdx = 0, url = urlBase.substr(0);
		for(var i = 0; i < urlParts.length; i++)
			url += "/" + (urlParts[i]? urlParts[i] : arguments[argIdx++]);
		
		var data = (typeof(arguments[argIdx]) === "function")? undefined : arguments[argIdx++];
		var callback = arguments[argIdx++];
		var context = arguments[argIdx];
		
		if(typeof(callback) !== "function")
		{
			var e = new Error("No callback function specified when requesting API \"" + url + "\"!");
			e.url = url;
			throw e;
		}
		
		$.ajax(
		{
			url: url,
			type: method,
			dataType: "json",
			data:
			{
				data: JSON.stringify(data)
			}

		}).done(function(data)
		{
			callback.call(context, undefined, data);

		}).fail(function()
		{
			callback.call(context, new Error("HTTP Error"), undefined);
		})
	});
}
//curry functions
DaggerAPIMethod.get = function(host, urlScheme){return(DaggerAPIMethod("GET", host, urlScheme));};
DaggerAPIMethod.post = function(host, urlScheme){return(DaggerAPIMethod("POST", host, urlScheme));};


////////////////////////////////////////////////////////////////

/*
var apiBase = "http://localhost:8000/v1";

var storeAPI =
{
	createOrder: DaggerAPIMethod.post(apiBase, "/orders"),
	getOrder: DaggerAPIMethod.get(apiBase, "/orders/:orderID"),
	modifyOrder: DaggerAPIMethod.post(apiBase, "/orders/:orderID"),
	checkoutOrder: DaggerAPIMethod.post(apiBase, "/orders/:orderid/checkout"),
};

function genericCompletion()
{
	console.log("done", this, arguments);
}
*/



var logiblockSettings =
{
	namingRules: {},
	locationOverride: undefined
};

function toTitleCase(word)
{
	if(logiblockSettings && logiblockSettings.namingRules[word])	return(logiblockSettings.namingRules[word]);
	return(word.substr(0, 1).toUpperCase() + word.substr(1));
}


jQuery(function($)
{
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
	

	var storeFrame = $('<iframe class="state" src="/state.html"></iframe>');
	storeFrame.load(function storeFrameLoaded(e)
	{
		window.cookies = e.target.contentWindow.cookies;

		$.resolve();
	});
	$("body").append(storeFrame);
});


jQuery.then(function($)
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


	!(function()
	{
		$("a.social-hn").each(function(i, a)
		{
			var $a = $(a), $p = $a.parent(), t;

			while(!t && !$p.is("body"))
			{
				t = $("h1", $p).html() || $("h2", $p).html() || $("h3", $p).html();
				$p = $p.parent();
			}

			t = t || document.title;
			$(a).attr("href", "https://news.ycombinator.com/submitlink?u=" + escape(window.location) + "&t=" + escape(t));
		});

	})();


	
	!(function()
	{
		var lastRead = (window.cookies.get("logiblog_last") || 0);

		//compare lastRead against blog entry publish times
		var unread = ((parseInt(lastRead) || 0) <= 1385637000)? 1 : 0;	//fake

		if(unread > 0)
		{
			var $sup = $('<sup class="badge"/>');

			var $blogNav = $('ul.navbar-nav a[href="/blog"]');

			$blogNav.append($sup.html(unread));
		}

	})();

});





/*
Store.prototype =
{
	getOrderID: function store_getOrderID()
	{
		return(window.cookies.get("currentOrder"));
	},

	setOrderID: function store_setOrderID(id)
	{
		window.cookies.set("currentOrder", id);
	},
	
	getTotal: function store_getTotal()
	{
		return(this.__total);
	},
	
	updateCart: function store_updateCart(order)
	{
		//update total
		var items = Object.keys(order.items), total = 0;
		for(var i = 0; i < items.length; i++)
		{
			var o = order.items[items[i]];
			total += o.qty * o.price;
		}
		this.__total = total;

		//update DOM
		$orderLink = $("a.order");
		$orderLink.attr("href", "/orders#" + order.id);
		var $qtyBadge = $(".cartQty", $orderLink);
		$qtyBadge.html(order.qty);
		if(order.qty > 0)	$qtyBadge.removeClass("hide");
		else				$qtyBadge.addClass("hide");
	}
};
function Store()
{
	this.__total = 0;
}

var store = new Store();


$.then(function()
{
	var orderID = window.cookies.get("currentOrder");
	if(orderID)
	{
		storeAPI.getOrder(orderID, function(err, order)
		{
			if(err)
				storeAPI.createOrder(function(err, order)
				{
					window.cookies.set("currentOrder", order.id);
					store.updateCart(order);
				});
			else
				store.updateCart(order);
		});
	}
	else
		storeAPI.createOrder(function(err, order)
		{
			window.cookies.set("currentOrder", order.id);
			store.updateCart(order);
		});
});
*/





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
