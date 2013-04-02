SC.Views.Login = Backbone.View.extend
  initialize: ->
    this.template = _.template $("#login-template").html()

  render: ->
    this.$el.html this.template({})
    this.el

  events:
    "click .login": "doLogin"

  doLogin: (e) ->
    e.preventDefault();
    email = this.$("input[name='email']").val();
    passwd = this.$("input[name='password']").val();
    # TODO: make this cross-browser - only works on Chrome or Mozilla for now
    auth_header = btoa(email + ":" + passwd)
    $("#logged-in-as").text(email);
    $("#logged-in-as").attr("href", "#account");
    SC.Models.system.fetch
      success: ->
        $("a.brand").text(SC.Models.system.attributes.name)
        window.location.replace("#")
