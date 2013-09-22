$(function($){

var $topNav = $("ul.navbar-nav li");

$topNav.click(function(e)
{
	var $e = $(e.currentTarget), href = $(e.target).attr("href");
	
	$topNav.removeClass("active");
	$e.addClass("active");
	
	console.log("nav to: " + href);

	e.preventDefault();
	return(false);
});


$.fn.animateUL = function animateUL(period)
{
	var $t = this, i = 0, interval = setInterval(function animateUL_onTimer()
	{
		$lis = $("li", $t);
		$lis.removeClass("active");
		$($lis[i]).addClass("active");
		if(++i >= $lis.length)	i = 0;
	}, period || 1000);

	return({$ul: this, stop: function(){clearInterval(interval);}});
}

$(".jumbotron ul.animated").animateUL(1500);

});
