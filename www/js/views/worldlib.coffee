SC.Views.WorldLib = Backbone.View.extend
  initialize: ->
    this.template = _.template $("#worldlib-template").html()

  render: ->
    this.$el.html this.template({})
    this.el
