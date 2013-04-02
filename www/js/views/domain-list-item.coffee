SC.Views.DomainListItem = Backbone.View.extend
  tagName: 'li'

  initialize: ->
    this.template = _.template $('#domain-list-item-template').html()
    this.model.bind 'change', this.render, this

  render: ->
    this.$el.html this.template this.model.toJSON()
    this.el
