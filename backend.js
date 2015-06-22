var Dagger = require("./Dagger");

var port = 8000;

var carts = {};

Products.prototype =
{
	get: function(request, response, args, callback)
	{
		callback(args);
	}
};
function Products()
{
	;
}

Cart.prototype =
{
	createOrGetOwn: function Cart_createOrGetOwn(request, response, args, callback)
	{
		if(request.method == "post")
		{
			callback(args);
		}
	},

	addItem: function Cart_addItem(request, response, args, callback)
	{
		if(request.method != "post")
			return(callback(undefined, 404));
		
		callback(args);
	},

};
function Cart()
{
	;
}

var products = new Products();

var cart = new Cart();

var server =
	new Dagger.Server(
	{
		"/": function root(request, response)
		{
			response.end("No.");
		},

		//api:
		"v1": new Dagger.Router(
		{
			"/products/:sku": Dagger.APIEndpoint(products.get, products),
			"/cart": Dagger.APIEndpoint(cart.createOrGetOwn, cart),
			"/cart/:cartId": Dagger.APIEndpoint(cart.createOrGetOwn, cart),
		})
	}, port);
