SC.Views.VerbListItem = Backbone.View.extend
  tagName: 'li'

  initialize: ->
    this.template = _.template $('#verb-list-item-template').html()
    this.model.bind 'change', this.render, this

  render: ->
    this.$el.html this.template this.model.toJSON()
    this.el

