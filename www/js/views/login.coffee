class SC.Views.Login extends Backbone.View
  initialize: ->
    @template = _.template $("#login-template").html()

  render: ->
    @$el.html @template({})
    @el

  events:
    "click .login": "doLogin"

  doLogin: (e) ->
    e.preventDefault();
    email = @$("input[name='email']").val();
    passwd = @$("input[name='password']").val();
    # TODO: make this cross-browser - only works on Chrome or Mozilla for now
    auth_header = btoa(email + ":" + passwd)
    $("#logged-in-as").text(email);
    $("#logged-in-as").attr("href", "#account");
    SC.Models.system.fetch
      success: ->
        $("a.brand").text(SC.Models.system.attributes.name)
        window.location.replace("#")
