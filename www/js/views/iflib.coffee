SC.Views.IFLib = Backbone.View.extend
  initialize: ->
    this.template = _.template $("#iflib-template").html()

  render: ->
    this.$el.html this.template({})
    this.el
